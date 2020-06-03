/* Yes! I'm including .c source files.
 * This gets the Arduino IDE to compile
 * without a Makefile and saves some memory.
 */

extern "C" {

#include "include/internals.h"
#include "softfloat/s_addCarryM.c"
#include "softfloat/s_addComplCarryM.c"
#include "softfloat/s_addMagsF64.c"
#include "softfloat/s_addM.c"
#include "softfloat/s_approxRecip32_1.c"
#include "softfloat/s_approxRecipSqrt32_1.c"
#include "softfloat/s_commonNaNToF32UI.c"
#include "softfloat/s_commonNaNToF64UI.c"
#include "softfloat/s_compare96M.c"
#include "softfloat/s_countLeadingZeros32.c"
#include "softfloat/s_countLeadingZeros64.c"
#include "softfloat/s_countLeadingZeros8.c"
#include "softfloat/s_f32UIToCommonNaN.c"
#include "softfloat/s_f64UIToCommonNaN.c"
#include "softfloat/s_mul64To128M.c"
#include "softfloat/s_mulAddF64.c"
#include "softfloat/s_negXM.c"
#include "softfloat/s_normRoundPackToF64.c"
#include "softfloat/s_normSubnormalF32Sig.c"
#include "softfloat/s_normSubnormalF64Sig.c"
#include "softfloat/s_propagateNaNF64UI.c"
#include "softfloat/s_remStepMBy32.c"
#include "softfloat/s_roundPackMToI64.c"
#include "softfloat/s_roundPackMToUI64.c"
#include "softfloat/s_roundPackToF32.c"
#include "softfloat/s_roundPackToF64.c"
#include "softfloat/s_roundPackToI32.c"
#include "softfloat/s_roundPackToUI32.c"
#include "softfloat/s_shiftLeftM.c"
#include "softfloat/s_shiftRightJam32.c"
#include "softfloat/s_shiftRightJam64.c"
#include "softfloat/s_shiftRightJamM.c"
#include "softfloat/s_shiftRightM.c"
#include "softfloat/s_shortShiftLeft64To96M.c"
#include "softfloat/s_shortShiftLeftM.c"
#include "softfloat/s_shortShiftRightJam64.c"
#include "softfloat/s_shortShiftRightJamM.c"
#include "softfloat/s_shortShiftRightM.c"
#include "softfloat/s_sub1XM.c"
#include "softfloat/s_subMagsF64.c"
#include "softfloat/s_subM.c"
#include "softfloat/softfloat_raiseFlags.c"
#include "softfloat/softfloat_state.c"
#include "softfloat/ui32_to_f64.c"
#include "softfloat/ui64_to_f64.c"
#include "softfloat/f64_add.c"
#include "softfloat/f64_div.c"
#include "softfloat/f64_eq.c"
#include "softfloat/f64_eq_signaling.c"
#include "softfloat/f64_isSignalingNaN.c"
#include "softfloat/f64_le.c"
#include "softfloat/f64_le_quiet.c"
#include "softfloat/f64_lt.c"
#include "softfloat/f64_lt_quiet.c"
#include "softfloat/f64_mulAdd.c"
#include "softfloat/f64_mul.c"
#include "softfloat/f64_rem.c"
#include "softfloat/f64_roundToInt.c"
#include "softfloat/f64_sqrt.c"
#include "softfloat/f64_sub.c"
#include "softfloat/f64_to_f32.c"
#include "softfloat/f64_to_i32.c"
#include "softfloat/f64_to_i32_r_minMag.c"
#include "softfloat/f64_to_i64.c"
#include "softfloat/f64_to_i64_r_minMag.c"
#include "softfloat/f64_to_ui32.c"
#include "softfloat/f64_to_ui32_r_minMag.c"
#include "softfloat/f64_to_ui64.c"
#include "softfloat/f64_to_ui64_r_minMag.c"
#include "softfloat/i32_to_f64.c"
#include "softfloat/i64_to_f64.c"
#include "softfloat/f32_to_f64.c"

}


