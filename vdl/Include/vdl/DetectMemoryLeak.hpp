#pragma once

//  メモリリーク詳細表示
#if defined _DEBUG 
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
