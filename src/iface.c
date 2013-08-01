#include <xdc/std.h>

#ifdef __TI_COMPILER_VERSION__
/* XDAIS Rule 13 - this #pragma should only apply to TI codegen */
#pragma CODE_SECTION(TRIK_VIDTRANSCODE_RESAMPLE_control, ".text:algControl")
#pragma CODE_SECTION(TRIK_VIDTRANSCODE_RESAMPLE_process, ".text:algProcess")
#pragma CODE_SECTION(TRIK_VIDTRANSCODE_RESAMPLE_initObj, ".text:algInit")
#pragma CODE_SECTION(TRIK_VIDTRANSCODE_RESAMPLE_free,    ".text:algFree")
#pragma CODE_SECTION(TRIK_VIDTRANSCODE_RESAMPLE_alloc,   ".text:algAlloc")
#endif

#include <limits.h>

#include "trik_vidtranscode_resample.h"
#include "internal/vidtranscode_resample_iface.h"
#include "internal/vidtranscode_resample_helpers.h"

char* s_dspInfoOutBuffer = NULL;
XDAS_Int32 s_dspDetectHueFrom = 0;
XDAS_Int32 s_dspDetectHueTo = 0;
XDAS_Int32 s_dspDetectSatFrom = 0;
XDAS_Int32 s_dspDetectSatTo = 0;
XDAS_Int32 s_dspDetectValFrom = 0;
XDAS_Int32 s_dspDetectValTo = 0;


#define TRIK_VIDTRANSCODE_RESAMPLE_IALGFXNS  \
    &TRIK_VIDTRANSCODE_RESAMPLE_IALG,		/* module ID */				\
    NULL,					/* activate */				\
    TRIK_VIDTRANSCODE_RESAMPLE_alloc,		/* alloc */				\
    NULL,					/* control (NULL => no control ops) */	\
    NULL,					/* deactivate */			\
    TRIK_VIDTRANSCODE_RESAMPLE_free,		/* free */				\
    TRIK_VIDTRANSCODE_RESAMPLE_initObj,		/* init */				\
    NULL,					/* moved */				\
    NULL					/* numAlloc (NULL => IALG_MAXMEMRECS) */

/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_IVIDTRANSCODE_RESAMPLE ========
 *  This structure defines TRIK's implementation of the IVIDTRANSCODE interface
 *  for the TRIK_VIDTRANSCODE_RESAMPLE module.
 */
IVIDTRANSCODE_Fxns TRIK_VIDTRANSCODE_RESAMPLE_IVIDTRANSCODE_RESAMPLE = {    /* module_vendor_interface */
    {TRIK_VIDTRANSCODE_RESAMPLE_IALGFXNS},
    TRIK_VIDTRANSCODE_RESAMPLE_process,
    TRIK_VIDTRANSCODE_RESAMPLE_control,
};

/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_IALG ========
 *  This structure defines TRIK's implementation of the IALG interface
 *  for the TRIK_VIDTRANSCODE_RESAMPLE module.
 */
#ifdef __TI_COMPILER_VERSION__
/* satisfy XDAIS symbol requirement without any overhead */
#if defined(__TI_ELFABI__) || defined(__TI_EABI_SUPPORT__)

/* Symbol doesn't have any leading underscores */
asm("TRIK_VIDTRANSCODE_RESAMPLE_IALG .set TRIK_VIDTRANSCODE_RESAMPLE_IVIDTRANSCODE_RESAMPLE");

#else

/* Symbol has a single leading underscore */
asm("_TRIK_VIDTRANSCODE_RESAMPLE_IALG .set _TRIK_VIDTRANSCODE_RESAMPLE_IVIDTRANSCODE_RESAMPLE");

#endif
#else

/*
 *  We duplicate the structure here to allow this code to be compiled and
 *  run using non-TI toolchains at the expense of unnecessary data space
 *  consumed by the definition below.
 */
IALG_Fxns TRIK_VIDTRANSCODE_RESAMPLE_IALG = {      /* module_vendor_interface */
    TRIK_VIDTRANSCODE_RESAMPLE_IALGFXNS
};

#endif




/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_alloc ========
 *  Return a table of memory descriptors that describe the memory needed
 *  to construct our object.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int TRIK_VIDTRANSCODE_RESAMPLE_alloc(
    const IALG_Params*		algParams,
    IALG_Fxns**			algFxns,
    IALG_MemRec			algMemTab[])
{
    /* Request memory for my object, ignoring algParams */
    algMemTab[0].size		= sizeof(TrikVideoResampleHandle);
    algMemTab[0].alignment	= 0;
    algMemTab[0].space		= IALG_EXTERNAL;
    algMemTab[0].attrs		= IALG_PERSIST;

    /* Return the number of records in the memTab */
    return 1;
}


/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  alloc operation above.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int TRIK_VIDTRANSCODE_RESAMPLE_free(
    IALG_Handle		algHandle,
    IALG_MemRec		algMemTab[])
{
    TrikVideoResampleHandle* handle = (TrikVideoResampleHandle*)algHandle;

    /* Returned data must match one returned in alloc */
    algMemTab[0].base		= handle;
    algMemTab[0].size		= sizeof(TrikVideoResampleHandle);
    algMemTab[0].alignment	= 0;
    algMemTab[0].space		= IALG_EXTERNAL;
    algMemTab[0].attrs		= IALG_PERSIST;

    /* Return the number of records in the memTab */
    return 1;
}


/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_initObj ========
 *  Initialize the memory allocated on our behalf (including our object).
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int TRIK_VIDTRANSCODE_RESAMPLE_initObj(
    IALG_Handle		algHandle,
    const IALG_MemRec	algMemTab[],
    IALG_Handle		algParent,
    const IALG_Params*	algParams)
{
    TrikVideoResampleHandle* handle = (TrikVideoResampleHandle*)algHandle;

    const TRIK_VIDTRANSCODE_RESAMPLE_Params* params = (TRIK_VIDTRANSCODE_RESAMPLE_Params*)algParams;
    if (params == NULL)
        params = getDefaultParams();

    handle->m_params = *params;
    handle->m_dynamicParams = *getDefaultDynamicParams();
    handleBuildDynamicParams(handle);
    if (!handleVerifyParams(handle))
        return IALG_EFAIL;

    return IALG_EOK;
}


/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_process ========
 */
XDAS_Int32 TRIK_VIDTRANSCODE_RESAMPLE_process(
    IVIDTRANSCODE_Handle	algHandle,
    XDM1_BufDesc*		xdmInBufs,
    XDM_BufDesc*		xdmOutBufs,
    IVIDTRANSCODE_InArgs*	vidInArgs,
    IVIDTRANSCODE_OutArgs*	vidOutArgs)
{
    TrikVideoResampleHandle* handle = (TrikVideoResampleHandle*)algHandle;
    XDM1_SingleBufDesc* xdmInBuf;
    XDAS_Int32 inBufFormat;
    XDAS_Int32 inBufHeight;
    XDAS_Int32 inBufWidth;
    XDAS_Int32 inBufLineLength;

    if (   (   (vidInArgs->size  != sizeof(IVIDTRANSCODE_InArgs))
            && (vidInArgs->size  != sizeof(TRIK_VIDTRANSCODE_RESAMPLE_InArgs)))
        || (vidOutArgs->size != sizeof(IVIDTRANSCODE_OutArgs)))
    {
        XDM_SETUNSUPPORTEDPARAM(vidOutArgs->extendedError);
        return IVIDTRANSCODE_EUNSUPPORTED;
    }

    if (vidInArgs->size == sizeof(TRIK_VIDTRANSCODE_RESAMPLE_InArgs))
    {
        const TRIK_VIDTRANSCODE_RESAMPLE_InArgs* inArgs = (const TRIK_VIDTRANSCODE_RESAMPLE_InArgs*)vidInArgs;
        s_dspDetectHueFrom = inArgs->detectHueFrom;
        s_dspDetectHueTo   = inArgs->detectHueTo;
        s_dspDetectSatFrom = inArgs->detectSatFrom;
        s_dspDetectSatTo   = inArgs->detectSatTo;
        s_dspDetectValFrom = inArgs->detectValFrom;
        s_dspDetectValTo   = inArgs->detectValTo;
    }
    else
    {
        s_dspDetectHueFrom = 0;
        s_dspDetectHueTo   = 0;
        s_dspDetectSatFrom = 0;
        s_dspDetectSatTo   = 0;
        s_dspDetectValFrom = 0;
        s_dspDetectValTo   = 0;
    }


    if (   xdmInBufs->numBufs != 1
        || handle->m_params.base.numOutputStreams < 0
        || xdmOutBufs->numBufs < handle->m_params.base.numOutputStreams)
    {
        XDM_SETUNSUPPORTEDPARAM(vidOutArgs->extendedError);
        return IVIDTRANSCODE_EFAIL;
    }


    xdmInBuf = &xdmInBufs->descs[0];
    if (   xdmInBuf->buf == NULL
        || vidInArgs->numBytes < 0
        || vidInArgs->numBytes > xdmInBuf->bufSize)
    {
        XDM_SETUNSUPPORTEDPARAM(vidOutArgs->extendedError);
        return IVIDTRANSCODE_EFAIL;
    }

    if (!handlePickInputParams(handle, &inBufFormat, &inBufHeight, &inBufWidth, &inBufLineLength))
    {
        XDM_SETUNSUPPORTEDPARAM(vidOutArgs->extendedError);
        return IVIDTRANSCODE_EFAIL;
    }

    XDM_CLEARACCESSMODE_WRITE(xdmInBuf->accessMask);
    XDM_SETACCESSMODE_READ(xdmInBuf->accessMask);

    vidOutArgs->bitsConsumed			= vidInArgs->numBytes * CHAR_BIT;
    vidOutArgs->decodedPictureType		= IVIDEO_NA_PICTURE;
    vidOutArgs->decodedPictureStructure		= IVIDEO_CONTENTTYPE_NA;
    vidOutArgs->decodedHeight			= handle->m_dynamicParams.inputHeight;
    vidOutArgs->decodedWidth			= handle->m_dynamicParams.inputWidth;

    s_dspInfoOutBuffer = NULL;
    switch (handle->m_params.base.numOutputStreams)
    {
      case 2:
        s_dspInfoOutBuffer = (char*)(xdmOutBufs->bufs[1]);
        s_dspInfoOutBuffer[0] = '\0';
        vidOutArgs->encodedBuf[1].buf        = xdmOutBufs->bufs[1];
        vidOutArgs->encodedBuf[1].bufSize    = xdmOutBufs->bufSizes[1];
        vidOutArgs->encodedBuf[1].accessMask = 0;
        XDM_SETACCESSMODE_WRITE(vidOutArgs->encodedBuf[1].accessMask);
        vidOutArgs->outputID[1]              = vidInArgs->inputID;
        vidOutArgs->bitsGenerated[1]         = xdmOutBufs->bufSizes[1] * CHAR_BIT;
        // fallthrough

      case 1:
      {
        XDM1_SingleBufDesc* xdmOutBuf = &vidOutArgs->encodedBuf[0];
        XDAS_Int32 outBufFormat;
        XDAS_Int32 outBufHeight;
        XDAS_Int32 outBufWidth;
        XDAS_Int32 outBufLineLength;
        XDAS_Int32 outBufUsed = 0;
        TrikVideoResampleStatus result;

        xdmOutBuf->buf		= xdmOutBufs->bufs[0];
        xdmOutBuf->bufSize	= xdmOutBufs->bufSizes[0];
        xdmOutBuf->accessMask	= 0;

        if (   xdmOutBuf->buf == NULL
            || xdmOutBuf->bufSize < 0)
        {
            XDM_SETCORRUPTEDDATA(vidOutArgs->extendedError);
            return IVIDTRANSCODE_EFAIL;
        }


        if (!handlePickOutputParams(handle, 0, &outBufFormat, &outBufHeight, &outBufWidth, &outBufLineLength))
        {
            XDM_SETUNSUPPORTEDPARAM(vidOutArgs->extendedError);
            return IVIDTRANSCODE_EFAIL;
        }

        result = resampleBuffer(xdmInBuf->buf, vidInArgs->numBytes,
                                inBufFormat, inBufHeight, inBufWidth, inBufLineLength,
                                xdmOutBuf->buf, xdmOutBuf->bufSize, &outBufUsed,
                                outBufFormat, outBufHeight, outBufWidth, outBufLineLength);
        switch (result)
        {
            case TRIK_VIDTRANSCODE_RESAMPLE_STATUS_OK:
                break;

            // TODO other statuses
            default:
                XDM_SETCORRUPTEDDATA(vidOutArgs->extendedError);
                return IVIDTRANSCODE_EFAIL;
        }

        XDM_SETACCESSMODE_WRITE(xdmOutBuf->accessMask);

        xdmOutBuf->bufSize				= outBufUsed;
        vidOutArgs->bitsGenerated[0]			= outBufUsed * CHAR_BIT;
        vidOutArgs->encodedPictureType[0]		= vidOutArgs->decodedPictureType;
        vidOutArgs->encodedPictureStructure[0]		= vidOutArgs->decodedPictureStructure;
        vidOutArgs->outputID[0]				= vidInArgs->inputID;
        vidOutArgs->inputFrameSkipTranscodeFlag[0]	= XDAS_FALSE;
        break;
      }

      default:
        return IVIDTRANSCODE_EUNSUPPORTED;
    }

    vidOutArgs->outBufsInUseFlag	= XDAS_FALSE;

    return IVIDTRANSCODE_EOK;
}


/*
 *  ======== TRIK_VIDTRANSCODE_RESAMPLE_control ========
 */
XDAS_Int32 TRIK_VIDTRANSCODE_RESAMPLE_control(
    IVIDTRANSCODE_Handle		algHandle,
    IVIDTRANSCODE_Cmd			vidCmd,
    IVIDTRANSCODE_DynamicParams*	vidDynParams,
    IVIDTRANSCODE_Status*		vidStatus)
{
    TrikVideoResampleHandle* handle = (TrikVideoResampleHandle*)algHandle;
    XDAS_Int32 retVal = IVIDTRANSCODE_EFAIL;

    /* initialize for the general case where we don't access the data buffer */
    XDM_CLEARACCESSMODE_READ(vidStatus->data.accessMask);
    XDM_CLEARACCESSMODE_WRITE(vidStatus->data.accessMask);

    switch (vidCmd)
    {
        case XDM_GETSTATUS:
        case XDM_GETBUFINFO:
            vidStatus->extendedError = 0;

            vidStatus->bufInfo.minNumInBufs = 1;
            vidStatus->bufInfo.minNumOutBufs = 1;
            vidStatus->bufInfo.minInBufSize[0] = 0;
            vidStatus->bufInfo.minOutBufSize[0] = 0;

            XDM_SETACCESSMODE_WRITE(vidStatus->data.accessMask);
            retVal = IVIDTRANSCODE_EOK;
            break;

        case XDM_SETPARAMS:
            if (vidDynParams->size == sizeof(IVIDTRANSCODE_DynamicParams))
            {
                handle->m_dynamicParams.base = *((IVIDTRANSCODE_DynamicParams*)vidDynParams);
                handleBuildDynamicParams(handle);
                retVal = handleVerifyParams(handle) ? IVIDTRANSCODE_EOK : IVIDTRANSCODE_EFAIL;
            }
            else if (vidDynParams->size == sizeof(TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams))
            {
                handle->m_dynamicParams = *((TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams*)vidDynParams);
                retVal = handleVerifyParams(handle) ? IVIDTRANSCODE_EOK : IVIDTRANSCODE_EFAIL;
            }
            else
                retVal = IVIDTRANSCODE_EUNSUPPORTED;
            break;

        case XDM_RESET:
        case XDM_SETDEFAULT:
            handle->m_params = *getDefaultParams();
            handle->m_dynamicParams = *getDefaultDynamicParams();
            handleBuildDynamicParams(handle);
            retVal = handleVerifyParams(handle) ? IVIDTRANSCODE_EOK : IVIDTRANSCODE_EFAIL;
            break;

        case XDM_FLUSH:
            retVal = IVIDTRANSCODE_EOK;
            break;

        case XDM_GETVERSION:
            if (reportVersion(vidStatus->data.buf, vidStatus->data.bufSize))
            {
                XDM_SETACCESSMODE_WRITE(vidStatus->data.accessMask);
                retVal = IVIDTRANSCODE_EOK;
            }
            else
                retVal = IVIDTRANSCODE_EFAIL;
            break;

        default:
            /* unsupported cmd */
            retVal = IVIDTRANSCODE_EFAIL;
            break;
    }

    return retVal;
}
