/**
  ******************************************************************************
  * @file           shell.h
  * @author         閸欍倓绠炵�癸拷
  * @brief          閸涙垝鎶ょ憴锝夊櫞閸ｃ劌銇旈弬鍥︽
  * 娴ｈ法鏁ゅ銉╊�冮敍锟�
  * <pre>
  * 娴ｈ法鏁ゅ銉╊�冮敍锟�
  *    0.閸掓繂顫愰崠鏍�栨禒鍫曞劥閸掑棎锟斤拷
  *    1.缂傛牕鍟撶涵顑挎鐎电懓绨查惃鍓塷id puts(char * buf , uint16_t len) 閸欐垿锟戒礁鍤遍弫鑸拷锟�
  *    2.shell_init(sign,puts) 閸掓繂顫愰崠鏍翻閸忋儲鐖ｈ箛妤�鎷版妯款吇鏉堟挸鍤妴锟�
  *    3.閺傛澘缂撴稉锟芥稉锟�  shellinput_t shellx , 閸掓繂顫愰崠鏍翻閸戯拷 shell_input_init(&shellx,puts,...);
  *    4.閹恒儲鏁归崚棰佺閸栧懏鏆熼幑顔兼倵閿涘矁鐨熼悽锟� shell_input(shellx,buf,len)
  *    *.  闂囷拷鐟曚焦鏁為崘灞芥嚒娴犮倕鍨拫鍐暏鐎癸拷 shell_register_command 鏉╂稖顢戝▔銊ュ斀閵嗭拷
  *    *.. shell_register_confirm() 閸欘垱鏁為崘灞界敨闁銆嶉崨鎴掓姢([Y/N]闁銆�)
  * </pre>
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 GoodMorning
  *
  ******************************************************************************
*/
#ifndef __SHELL_H__
#define __SHELL_H__

// 娴犮儰绗呮稉锟� shell 閹碉拷娓氭繆绂嗛惃鍕唨閺堫剙绨�
#include "ustdio.h"
#include <stdio.h>

/* Public macro (閸忚鲸婀佺�癸拷)------------------------------------------------------------*/

/* ---- option (闁板秶鐤嗘い锟�) ---- */

/**
 * @brief 娑擄拷 1 閺冩湹濞囬懗钘夐挬鐞涒�茬癌閸欏鐖叉潻娑滎攽閺屻儲澹橀崠褰掑帳閿涘苯鎯侀崚娆庡▏閻€劌宕熼柧鎹愩�冮妴锟�
 * @note  閻€劋绨╅崣澶嬬埐閺嬪嫬缂撻弻銉嚄缁崵绮虹憰浣圭槷闁炬崘銆冭箛顐礉娴ｅ棝娓剁憰浣稿閸忥拷 avltree.c 閺傚洣娆㈤弨顖涘瘮閿涘奔绱�
 *        婢舵氨绱拠鎴濄亣濮掞拷 800~1000bytes 閻拷 ROM 閸楃姷鏁ら妴鍌烇拷鍌滄暏娴滃孩鏁為崘宀�娈戦崨鎴掓姢鏉堝啫顦块弮璺烘儙閻€劊锟斤拷
*/
#define USE_AVL_TREE            0

/// 閸涙垝鎶ょ敮锔跨瑐閸欏倹鏆熼惃鍕摟缁楋缚瑕嗘潏鎾冲弳閺堬拷闂�鑳唶瑜版洟鏆辨惔锟�
#define COMMANDLINE_MAX_LEN     100

/// 閹貉冨煑閸欐媽顔囪ぐ鏇熸蒋閻╊喗鏆熼敍宀冾啎娑擄拷 0 閺冩湹绗夌拋鏉跨秿
#define COMMANDLINE_MAX_RECORD  4

/* ---- option end ---- */

// 娑擄拷娴滄盯鏁崐纭风窗
#define KEYCODE_END               35
#define KEYCODE_HOME              36
#define KEYCODE_CTRL_C            0x03
#define KEYCODE_BACKSPACE         0x08   //闁款喚娲忛惃鍕礀闁拷闁匡拷
#define KEYCODE_TAB               '\t'   //闁款喚娲忛惃鍓嘺b闁匡拷
#define KEYCODE_NEWLINE           0x0A
#define KEYCODE_ENTER             0x0D   //闁款喚娲忛惃鍕礀鏉烇箓鏁�
#define KEYCODE_ESC               0x1b


#define MODIFY_MASK  0xABCD4320
#define FUNC_NAME(_1, _2, _3, NAME,...) NAME
#define FUNC(...)                       FUNC_NAME(__VA_ARGS__,PRINTF3,PRINTF2)(__VA_ARGS__)


/**
  * @author   閸欍倓绠炵�癸拷
  * @brief    瀵帮拷閹貉冨煑閸欑増鏁為崘灞芥嚒娴狅拷
  * @note     鐠嬪啰鏁ょ�瑰繑鏁為崘灞芥嚒娴犮倗娈戦崥灞炬娴兼碍鏌婂杞扮娑擃亙绗岄崨鎴掓姢鐎电懓绨查惃鍕付閸掕泛娼�
  * @param    name  : 閸氬秶袨閿涘苯绻�妞よ璐熺敮鎼佸櫤鐎涙顑佹稉鍙夊瘹闁斤拷
  * @param    func  : 閸涙垝鎶ら幍褑顢戦崙鑺ユ殶閿涘瓐see cmd_fn_t
*/
#define shell_register_command(name,func)\
do{\
	static struct shellcommand newcmd = {0};\
	_shell_register(&newcmd,name,func);     \
}while(0)


/**
  * @author   閸欍倓绠炵�癸拷
  * @brief    瀵帮拷閹貉冨煑閸欑増鏁為崘灞肩娑擃亜鐢柅澶愩�嶉崨鎴掓姢閿涘矂娓剁憰浣界翻閸忥拷 [Y/N/y/n] 閹靛秵澧界悰灞筋嚠鎼存梻娈戦崨鎴掓姢
  * @note     鐠嬪啰鏁ょ�瑰繑鏁為崘灞芥嚒娴犮倗娈戦崥灞炬娴兼碍鏌婂杞扮娑擃亙绗岄崨鎴掓姢鐎电懓绨查惃鍕付閸掕泛娼�
  * @param    name  : 閸氬秶袨閿涘苯绻�妞よ璐熺敮鎼佸櫤鐎涙顑佹稉鍙夊瘹闁斤拷
  * @param    func  : 閸涙垝鎶ら幍褑顢戦崙鑺ユ殶閿涘瓐see cmd_fn_t
*/
#define shell_register_confirm(name,func,info)\
do{\
	static struct shellconfirm confirm = {  \
	.prompt = info ,.flag = CONFIRM_FLAG }; \
	_shell_register(&confirm.cmd,name,func);\
}while(0)


/// 娴犮儰绗呮稉锟� shell_input_init() 閹碉拷閻€劌鐣�
#define MODIFY_SIGN (MODIFY_MASK|0x1)
#define MODIFY_GETS (MODIFY_MASK|0x2)

/// 閸樺棗褰堕柆妤冩殌闂傤噣顣介敍灞藉悑鐎硅妫悧鍫熸拱娴狅絿鐖�
#define SHELL_INPUT_INIT(...) shell_input_init(__VA_ARGS__)


/// shell 閸忋儱褰涚�电懓绨查崙鍝勫經閿涘奔绮犻崫顏堝櫡鏉堟挸鍙嗛崚娆庣矤鐎电懓绨查惃鍕勾閺傜绶崙锟�
#define shell_input(shellin,buf,len) \
do{\
	if ((shellin)->gets) {\
		current_puts = (shellin)->puts;        \
		(shellin)->gets((shellin),(buf),(len));\
		current_puts = default_puts;           \
	}\
}while(0)


/* Public types ------------------------------------------------------------*/

enum INPUT_PARAMETER {
	PARAMETER_ERROR = -2,
	PARAMETER_HELP = -1,
};


/// 閸涙垝鎶ょ�电懓绨查惃鍕毐閺佹壆琚崹瀣剁礉閼峰厖绨稉杞扮矆娑斿牐绶崗銉啎鐠佲�茶礋 void *,閹存垳绗夌拋鏉跨繁娴滐拷
typedef void (*cmd_fn_t)(void * arg);


#if USE_AVL_TREE     // 閸涙垝鎶ょ槐銏犵穿閻⑩暆vl閺嶆垼绻樼悰灞剧叀閹垫儳灏柊宥嗘闂囷拷鐟曪拷 avltree.c 閺�顖涘瘮
	#include "avltree.h"
	typedef struct avl_node cmd_entry_t ;
	typedef struct avl_root cmd_root_t ;
#else                   // 閸楁洟鎽肩悰銊ㄥΝ閻愮櫢绱濋悽銊︽降娑撴彃鎳℃禒锟�
	struct slist{struct slist * next;} ;
	typedef struct slist cmd_entry_t ;
	typedef struct slist cmd_root_t ;
#endif


/// 閸涙垝鎶ょ紒鎾寸�担鎿勭礉閻€劋绨▔銊ュ斀閸栧綊鍘ら崨鎴掓姢
typedef struct shellcommand {
	cmd_entry_t   node    ; ///< 閸涙垝鎶ょ槐銏犵穿閹恒儱鍙嗛悙鐧哥礉閻€劑鎽肩悰銊﹀灗娴滃苯寮堕弽鎴濐嚠閸涙垝鎶ゆ担婊堟肠閸氾拷
	char *        name    ; ///< 鐠佹澘缍嶅В蹇旀蒋閸涙垝鎶ょ�涙顑佹稉鑼畱閸愬懎鐡ㄩ崷鏉挎絻
	cmd_fn_t      func    ; ///< 鐠佹澘缍嶉崨鎴掓姢閸戣姤鏆� cmd_fn_t 鐎电懓绨查惃鍕敶鐎涙ê婀撮崸锟�
	unsigned int  ID      ; ///< 鐎碉拷 name 鐎涙顑佹稉鑼剁箻鐞涘苯甯囩紓鈺佺繁閸掓壆娈� ID 閸欏嚖绱濋崠褰掑帳閺佹澘鐡уВ鏂跨摟缁楋缚瑕嗛弫鍫㈠芳妤傛ǜ锟斤拷
}
shellcmd_t;


/// 鐢妇鈥樼拋銈夛拷澶愩�嶉惃鍕嚒娴犮倗绮ㄩ弸鍕秼
typedef struct shellconfirm {
	struct shellcommand  cmd; /// 鐎电懓绨查惃鍕嚒娴犮倕褰块崘鍛摠
	char * prompt ;
	#define CONFIRM_FLAG 0x87654321U
	size_t flag   ;           /// 绾喛顓婚幓鎰仛娣団剝浼�
}
shellcfm_t ;

/// 娴溿倓绨扮紒鎾寸�担鎿勭礉閺佺増宓侀惃鍕翻閸忋儴绶崙杞扮瑝娑擄拷鐎癸拷
typedef struct shell_input
{
	/// 閹稿洤鐣鹃弫鐗堝祦濞翠浇绶崗锟�,閸掓繂顫愰崠鏍帛鐠併倓璐� cmdline_gets() ,閸楀啿鎳℃禒銈堫攽
	void (*gets)(struct shell_input * , char * ,int );

	/// 閹稿洤鐣鹃弫鐗堝祦濞翠礁顕惔鏃傛畱鏉堟挸鍤幒銉ュ經閿涘奔瑕嗛崣锝嗗灗閼帮拷 telnet 鏉堟挸鍤粵锟�
	fmt_puts_t puts;

	/// app閸欘垳鏁ら崣鍌涙殶閿涘瞼鍩嶉幀搴濈疄閻€劌姘ㄩ幀搴濈疄閻拷
	void *  apparg;

	/// 閸涙垝鎶ょ悰宀冪翻閸忋儳顑侀崣锟�
	char    sign[COMMANDLINE_MAX_LEN];

	// 閸涙垝鎶ょ悰宀�娴夐崗宕囨畱閸欏倹鏆�
	char          cmdline[COMMANDLINE_MAX_LEN]; ///< 閸涙垝鎶ょ悰灞藉敶鐎涳拷
	unsigned char edit                        ; ///< 瑜版挸澧犻崨鎴掓姢鐞涘瞼绱潏鎴滅秴缂冿拷
	unsigned char tail                        ; ///< 瑜版挸澧犻崨鎴掓姢鐞涘矁绶崗銉х波鐏忥拷 tail

	#if (COMMANDLINE_MAX_RECORD) //婵″倹鐏夌�规矮绠熸禍鍡楀坊閸欒尙閭ぐ锟�
		unsigned char htywrt  ;  ///< 閸樺棗褰剁拋鏉跨秿閸愶拷
		unsigned char htyread ; ///< 閸樺棗褰剁拋鏉跨秿鐠囷拷
		char    history[COMMANDLINE_MAX_RECORD][COMMANDLINE_MAX_LEN]; ///< 閸樺棗褰剁拋鏉跨秿閸愬懎鐡�
	#endif
}
shellinput_t;


typedef	void (*shellgets_t)(struct shell_input * , char * ,int );

/* Public variables ---------------------------------------------------------*/

extern char DEFAULT_INPUTSIGN[]; // 姒涙顓绘禍銈勭鞍閺嶅洤绻�


/* Public function prototypes 鐎电懓顦婚崣顖滄暏閹恒儱褰� -----------------------------------*/

//濞夈劌鍞介崨鎴掓姢閿涘矁绻栨稉顏勫毐閺侀绔撮懜顑跨瑝閻╁瓨甯寸拫鍐暏閿涘瞼鏁ょ�癸拷 shell_register_command() 闂傚瓨甯寸拫鍐暏
void _shell_register(struct shellcommand * newcmd,char * cmd_name, cmd_fn_t cmd_func);


/**
  * @author   閸欍倓绠炵�癸拷
  * @brief    绾兛娆㈡稉濠冨复閺�璺哄煂閻ㄥ嫭鏆熼幑顔煎煂閸涙垝鎶ょ悰宀�娈戞导鐘虹翻
  * @param    shellin : 娴溿倓绨�
  * @param    recv    : 绾兛娆㈢仦鍌涘閹恒儲鏁归崚鎵畱閺佺増宓佺紓鎾冲暱閸栧搫婀撮崸锟�
  * @param    len     : 绾兛娆㈢仦鍌涘閹恒儲鏁归崚鎵畱閺佺増宓侀梹鍨
  * @return   void
*/
void cmdline_gets(struct shell_input * ,char * ,int );

//鐟欙絾鐎介崨鎴掓姢鐞涘苯寮弫鎵祲閸忓啿濮涢懗钘夊毐閺侊拷
/**
  * @brief    鏉烆剚宕查懢宄板絿閸涙垝鎶ら崣宄版倵闂堛垻娈戞潏鎾冲弳閸欏倹鏆熼敍灞界摟缁楋缚瑕嗘潪顑胯礋閺佸瓨鏆�
  * @param    str     閸涙垝鎶ょ�涙顑佹稉鎻掓倵闂堛垺澧嶇捄鐔峰棘閺佹壆绱﹂崘鎻掑隘閹稿洭鎷�
  * @param    argv    閺佺増宓佹潪顒佸床閸氬海绱︾�涙ê婀撮崸锟�
  * @param    maxread 閺堬拷婢堆嗩嚢閸欐牗鏆�
  * @return   閺佺増宓佹稉顏呮殶
	  * @retval   >= 0         鐠囪褰囬崨鎴掓姢閸氬酣娼伴幍锟界捄鐔峰棘閺侀閲滈弫锟�
	  * @retval   PARAMETER_ERROR(-2)  閸涙垝鎶ら崥搴ㄦ桨閹碉拷鐠虹喎寮弫鐗堟箒鐠囷拷
	  * @retval   PARAMETER_HELP(-1)   閸涙垝鎶ら崥搴ㄦ桨鐠虹喍绨� ? 閸欙拷
*/
int  cmdline_param(char * str,int * argv,int maxread);

/**
  * @author   閸欍倓绠炵�癸拷
  * @brief    閹讹拷 "a b c d" 閺嶇厧绱￠崠鏍ㄥ絹閸欐牔璐� char*argv[] = {"a","b","c","d"};娴犮儰绶礸etopt()鐟欙絾鐎�
  * @param    str    : 閸涙垝鎶ょ�涙顑佹稉鎻掓倵闂堛垺澧嶇捄鐔峰棘閺佹壆绱﹂崘鎻掑隘閹稿洭鎷�
  * @param    argv   : 閺佺増宓佹潪顒佸床閸氬海绱︾�涙ê婀撮崸锟�
  * @param    maxread: 閺堬拷婢堆嗩嚢閸欐牗鏆�
  * @return   閺堬拷缂佸牐顕伴崣鏍у棘閺侀閲滈弫鎷岀翻閸戯拷
*/
int  cmdline_strtok(char * str ,char ** argv ,int maxread);

// 閸掓繂顫愰崠鏍祲閸忓啿鍤遍弫锟�

void shell_start(struct shell_input * shellin, char *welcome_str);

/**
  * @author   閸欍倓绠炵�癸拷
  * @brief    shell 閸掓繂顫愰崠锟�,濞夈劌鍞介崙鐘虫蒋閸╃儤婀伴惃鍕嚒娴犮們锟藉倸鍘戠拋闀愮瑝閸掓繂顫愰崠鏍ワ拷锟�
  * @param    defaultsign : 闁插秴鐣炬稊澶愮帛鐠併倛绶崙鐑樼垼韫囨绱濇稉锟� NULL 閸掓瑤绗夋穱顔芥暭姒涙顓婚弽鍥х箶
  * @param    puts        : printf,printk,printl 閻ㄥ嫰绮拋銈堢翻閸戠尨绱濇俊鍌欑矤娑撴彃褰涙潏鎾冲毉閿涘奔璐� NULL 閸掓瑤绗夐幍鎾冲祪娣団剝浼呴妴锟�
  * @return   don't care
*/
void shell_init(char * defaultsign ,fmt_puts_t puts);

/**
  * @author   閸欍倓绠炵�癸拷
  * @brief    閸掓繂顫愰崠鏍︾娑擄拷 shell 娴溿倓绨伴敍宀勭帛鐠併倛绶崗銉よ礋 cmdline_gets
  * @param    shellin   : 闂囷拷鐟曚礁鍨垫慨瀣閻拷 shell 娴溿倓绨�
  * @param    shellputs : shell 鐎电懓绨叉潏鎾冲毉閿涘苯顩ф禒搴濊閸欙綀绶崙鎭掞拷锟�
  * @param    ...       : 鐎碉拷 gets 閸滐拷 sign 闁插秴鐣炬稊澶涚礉婵″倽鎷烽崝锟� MODIFY_SIGN,"shell>>"
  * @return   don't care
*/
void shell_input_init(struct shell_input * shellin , fmt_puts_t shellputs,...);


/**
  * @brief    閸涙垝鎶ょ悰灞间繆閹垳鈥樼拋銈忕礉婵″倹鐏夋潏鎾冲弳 y/Y 閸掓瑦澧界悰灞芥嚒娴狅拷
  * @param    shell  : 鏉堟挸鍙嗘禍銈勭鞍
  * @param    info   : 闁銆嶆穱鈩冧紖
  * @param    yestodo: 鏉堟挸鍙� y/Y 閸氬孩澧嶉棁锟介幍褑顢戦惃鍕嚒娴狅拷
  * @return   void
*/
void shell_confirm(struct shell_input * shellin ,char * info ,cmd_fn_t yestodo) ;
#endif
