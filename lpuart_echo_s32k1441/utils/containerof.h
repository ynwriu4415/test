#ifndef container_of
//------------------------------------------------------------------
#ifndef offsetof
	//#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)// 鑾峰彇"MEMBER鎴愬憳"鍦�"缁撴瀯浣揟YPE"涓殑浣嶇疆鍋忕Щ
	#include <stddef.h>
#endif

#ifndef __GNUC__
	// 鏍规嵁"缁撴瀯浣�(type)鍙橀噺"涓殑"鍩熸垚鍛樺彉閲�(member)鐨勬寚閽�(ptr)"鏉ヨ幏鍙栨寚鍚戞暣涓粨鏋勪綋鍙橀噺鐨勬寚閽�
	#define container_of(ptr, type, member)  ((type*)((char*)ptr - offsetof(type, member)))
	// 姝ゅ畯瀹氫箟鍘熸枃涓� GNU C 鎵�鍐欙紝濡備笅锛屾湁浜涚紪璇戝櫒鍙敮鎸� ANSI C /C99 鐨勶紝鎵�浠ヤ綔浠ヤ笂淇敼
#else
	#define container_of(ptr, type, member) ((type*)((char*)ptr - offsetof(type, member)))
#endif

//------------------------------------------------------------------
#endif
