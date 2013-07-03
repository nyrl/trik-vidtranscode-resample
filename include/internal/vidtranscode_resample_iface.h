#ifndef TRIK_VIDTRANSCODE_RESAMPLE_INTERNAL_VIDTRANSCODE_RESAMPLE_IFACE_H_
#define TRIK_VIDTRANSCODE_RESAMPLE_INTERNAL_VIDTRANSCODE_RESAMPLE_IFACE_H_

#include <xdc/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividtranscode.h>

#include "trik_vidtranscode_resample.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef struct TrikVideoResampleHandle {
    IALG_Obj					m_alg;	/* MUST be first field of all XDAIS algs */

    TRIK_VIDTRANSCODE_RESAMPLE_Params		m_params;
    TRIK_VIDTRANSCODE_RESAMPLE_DynamicParams	m_dynamicParams;
} TrikVideoResampleHandle;


extern Int TRIK_VIDTRANSCODE_RESAMPLE_alloc(
        const IALG_Params*	algParams,
        IALG_Fxns**		algFxns,
        IALG_MemRec		algMemTab[]);

extern Int TRIK_VIDTRANSCODE_RESAMPLE_free(
        IALG_Handle		algHandle,
        IALG_MemRec		algMemTab[]);

extern Int TRIK_VIDTRANSCODE_RESAMPLE_initObj(
        IALG_Handle		algHandle,
        const IALG_MemRec	algMemTab[],
        IALG_Handle		algParent,
        const IALG_Params*	algParams);

extern XDAS_Int32 TRIK_VIDTRANSCODE_RESAMPLE_process(
        IVIDTRANSCODE_Handle		algHandle,
        XDM1_BufDesc*			xdmInBufs,
        XDM_BufDesc*			xdmOutBufs,
        IVIDTRANSCODE_InArgs*		vidInArgs,
        IVIDTRANSCODE_OutArgs*		vidOutArgs);

extern XDAS_Int32 TRIK_VIDTRANSCODE_RESAMPLE_control(
        IVIDTRANSCODE_Handle		algHandle,
        IVIDTRANSCODE_Cmd		vidCmd,
        IVIDTRANSCODE_DynamicParams*	vidDynamicParams,
        IVIDTRANSCODE_Status*		vidStatus);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // !TRIK_VIDTRANSCODE_RESAMPLE_INTERNAL_VIDTRANSCODE_RESAMPLE_IFACE_H_
