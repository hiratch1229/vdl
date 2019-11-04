#pragma once

//  ƒƒ‚ƒŠƒŠ[ƒNÚ×•\¦
#if defined(DEBUG) | defined(_DEBUG)
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
