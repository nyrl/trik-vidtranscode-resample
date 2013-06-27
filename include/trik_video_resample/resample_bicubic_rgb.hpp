#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_BICUBIC_RGB_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_BICUBIC_RGB_HPP_

#ifndef __cplusplus
#error C++-only header
#endif

#include <algorithm>

#include "include/trik_video_resample/resample_cubic.hpp"
#include "include/trik_video_resample/resample_rgb.hpp"


template <typename UByteType,
          typename RGBAccessorSrc,
          typename RGBAccessorDst>
class TrikRGBBicubicResampler
{
  static const size_t Rows   = 4;
  static const size_t Cols   = 4;
  static const size_t Colors = 3;
  static const size_t ColorR = 0;
  static const size_t ColorG = 1;
  static const size_t ColorB = 2;


  protected:
    static bool fetchRGBColumn(const float      _rowWeight[Rows],		// bicubic row weight matrix
                               const UByteType* _src[Rows],			// row[-1, 0, 1, 2]
                               float            _rgbColumn[Colors])		// [red, green, blue]
    {
      float rgbRows[Colors*Rows];

      for (size_t row = 0; row < Rows; ++row)
        _src[row] = RGBAccessorSrc::read(_src[row],
                                         rgbRows[row + ColorR*Rows],
                                         rgbRows[row + ColorG*Rows],
                                         rgbRows[row + ColorB*Rows]);

      return trikCubicInterpolationValue<Colors>(_rowWeight, rgbRows, _rgbColumn);
    }

    static bool zeroRGBColumn(float _rgbColumn[Colors]) // [red, green, blue]
    {
      for (size_t clr = 0; clr < Colors; ++clr)
        _rgbColumn[clr] = 0.0;
      return true;
    }

    static bool copyRGBColumn(const float _rgbColumnFrom[Colors],	// [red, green, blue]
                              float       _rgbColumnTo[Colors])		// [red, green, blue]
    {
      for (size_t clr = 0; clr < Colors; ++clr)
        _rgbColumnTo[clr] = _rgbColumnFrom[clr];
      return true;
    }


    static bool initRGBColumns(const float      _rowWeight[Rows],			// bicubic row weight matrix
                               const UByteType* _src[Rows],				// row[-1, 0, 1, 2]
                               size_t&          _currentColumn,				// always 0 on return
                               size_t&          _remainColumns,				// total amount of columns, amount of unprocessed columns on return
                               float            _rgbColumns[Colors*Cols])		// [rgb[col[-1]], rgb[col[0]], rgb[col[1]], rgb[col[2]]]
    {
      _currentColumn = 0;

      // column[0]
      if (_remainColumns < 1)
      {
        if (!zeroRGBColumn(_rgbColumns + 1*Colors))
          return false;
      }
      else
      {
        if (!fetchRGBColumn(_rowWeight, _src, _rgbColumns + 1*Colors))
          return false;
        --_remainColumns;
      }

      // column[-1] = column[0]
      if (!copyRGBColumn(_rgbColumns + 1*Colors, _rgbColumns + 0*Colors))
        return false;

      // column[1...]
      for (size_t col = 2; col < Cols; ++col)
        if (_remainColumns > 0)
        {
          if (!fetchRGBColumn(_rowWeight, _src, _rgbColumns + col*Colors))
            return false;
          --_remainColumns;
        }
        else
        {
          if (!copyRGBColumn(_rgbColumns + (col-1)*Colors, _rgbColumns + col*Colors))
            return false;
        }

      return true;
    }

    // TODO subject to optimization - use larger rgbComponents buffer and reduce number of shifts
    static bool updateRGBColumns(const float      _rowWeight[Rows],			// bicubic row weight matrix
                                 const UByteType* _src[Rows],				// row[-1, 0, 1, 2]
                                 size_t&          _currentColumn,			// current column
                                 size_t           _desiredColumn,			// desired src column (col[0])
                                 size_t&          _remainColumns,			// amount of unprocessed columns
                                 float            _rgbColumns[Colors*Cols])		// [rgb[col[-1]], rgb[col[0]], rgb[col[1]], rgb[col[2]]]
    {
      for (/*_currentColumn*/; _currentColumn < _desiredColumn; ++_currentColumn)
      {
        // shift left, not optimal
        for (size_t col = 0; col < Cols-1; ++col)
          if (!copyRGBColumn(_rgbColumns + (col+1)*Colors, _rgbColumns + col*Colors))
            return false;

        if (_remainColumns > 0) // fetch if not reached end yet
        {
          if (!fetchRGBColumn(_rowWeight, _src, _rgbColumns + (Cols-1)*Colors))
            return false;
          --_remainColumns;
        } // keep last otherwise
      }

      return true;
    }


    static bool resampleRow(float               _rowFactor,
                            const UByteType*    _src[Rows],		// row[-1, 0, 1, 2]
                            size_t              _srcColumns,		// amount of columns in _src
                            UByteType*           _dst,			// single row
                            size_t              _dstColumns)		// amount of columns in _dst
    {
      float rowWeight[Rows];
      if (!trikCubicInterpolationWeight(_rowFactor, rowWeight))
        return false;

      size_t srcCurrentColumn;
      size_t srcRemainColumns = _srcColumns;
      float rgbColumns[Colors*Cols];
      if (!initRGBColumns(rowWeight, _src, srcCurrentColumn, srcRemainColumns, rgbColumns))
        return false;

      const float src2dstColumnsFactor = static_cast<float>(_srcColumns) / static_cast<float>(_dstColumns);
      for (size_t dstColumn = 0; dstColumn < _dstColumns; ++dstColumn)
      {
        const float srcColumn = src2dstColumnsFactor * dstColumn;
        const size_t srcColumnInt = srcColumn; // truncated
        const float srcColumnFactor = srcColumn - srcColumnInt;

        if (!updateRGBColumns(rowWeight, _src, srcCurrentColumn, srcColumnInt, srcRemainColumns, rgbColumns))
          return false;

        float colWeight[Cols];
        if (!trikCubicInterpolationWeight(srcColumnFactor, colWeight))
          return false;

        float rgbPixel[Colors];
        if (!trikCubicInterpolationValue<Colors>(colWeight, rgbColumns, rgbPixel))
          return false;

        _dst = RGBAccessorDst::write(_dst, rgbPixel[ColorR], rgbPixel[ColorG], rgbPixel[ColorB]);
      }

      return true;
    }


    static bool getRowPtr(UByteType*  _buffer,
                          UByteType*& _rowPtr,
                          size_t        _row,
                          size_t        _rows,
                          size_t        _lineLength)
    {
      if (_row >= _rows)
        return false;
      _rowPtr = _buffer + _lineLength;
      return true;
    }

    static bool getRowPtr(const UByteType*  _buffer,
                          const UByteType*& _rowPtr,
                          size_t            _row,
                          size_t            _rows,
                          size_t            _lineLength)
    {
      if (_row >= _rows)
        return false;
      _rowPtr = _buffer + _lineLength;
      return true;
    }

    template <typename UByteTypeCV>
    static bool getRowsRangePtr(UByteTypeCV*  _buffer,
                                UByteTypeCV*  _rowPtr[Rows],
                                size_t        _baseRow,
                                size_t        _rows,
                                size_t        _lineLength)
    {
      if (_rows == 0)
        return false;

      const size_t idx0 = std::min(((_baseRow == 0) ? 0 : _baseRow-1), (_rows-1));
      if (!getRowPtr(_buffer, _rowPtr[0], idx0, _rows, _lineLength))
        return false;

      for (size_t ri = 1; ri < Rows; ++ri)
      {
        const size_t idx = std::min((_baseRow + ri), (_rows-1));
        if (!getRowPtr(_buffer, _rowPtr[ri], idx, _rows, _lineLength))
          return false;
      }

      return true;
    }


  public:
    static bool resampleImage(const UByteType*    _src,
                              size_t              _srcRows,
                              size_t              _srcColumns,
                              size_t              _srcLineLength,
                              UByteType*          _dst,
                              size_t              _dstRows,
                              size_t              _dstColumns,
                              size_t              _dstLineLength)
    {
      const float src2dstRowFactor = static_cast<float>(_srcRows) / static_cast<float>(_dstRows);
      for (size_t dstRow = 0; dstRow < _dstRows; ++dstRow)
      {
        const float srcRow = src2dstRowFactor * dstRow;
        const size_t srcRowInt = srcRow; // truncated
        const float srcRowFactor = srcRow - srcRowInt;

        UByteType* dstRowPtr;
        const UByteType* srcRowPtrs[Rows];
        if (   !getRowPtr(_dst, dstRowPtr, dstRow, _dstRows, _dstLineLength)
            || !getRowsRangePtr(_src, srcRowPtrs, srcRowInt, _srcRows, _srcLineLength))
          return false;

        if (!resampleRow(srcRowFactor, srcRowPtrs, _srcColumns, dstRowPtr, _dstColumns))
          return false;
      }

      return true;
    }

};

#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_RESAMPLE_BICUBIC_RGB_HPP_
