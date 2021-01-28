
/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "shell.h"
#include "containerof.h"

/* Private types ------------------------------------------------------------*/

union uncmd {
	struct {// 鍛戒护鍙峰垎涓轰互涓嬩簲涓儴鍒�
		unsigned int CRC2      : 8;
		unsigned int CRC1      : 8;///< 浣庡崄鍏綅涓轰袱涓� crc 鏍￠獙鐮�
		unsigned int Sum       : 5;///< 鍛戒护瀛楃鐨勬�诲拰
		unsigned int Len       : 5;///< 鍛戒护瀛楃鐨勯暱搴︼紝5 bit 锛屽嵆鍛戒护闀垮害涓嶈兘瓒呰繃31涓瓧绗�
		unsigned int FirstChar : 6;///< 鍛戒护瀛楃鐨勭涓�涓瓧绗�
	}part;
	unsigned int ID;               ///< 鐢辨鍚堝苟涓� 32 浣嶇殑鍛戒护鐮�
};

/* Private macro ------------------------------------------------------------*/

#define VERSION      "V2.0.4"

#if USE_AVL_TREE 
	#define NEXT(x)          avl_next(x)
	#define FIRST(root)      avl_first(root)
	#define ROOT(root)       ((root)->avl_node)
#else 
	#define NEXT(x)          ((x)->next)
	#define FIRST(root)      ((root)->next)
	#define ROOT(root)       ((root)->next)
#endif

/* Private variables --------------------------------------------------------*/

static const  unsigned char F_CRC8_Table[256] = {//姝ｅ簭,楂樹綅鍏堣 x^8+x^5+x^4+1
	0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 0xb9, 0x88, 0xdb, 0xea, 0x7d, 0x4c, 0x1f, 0x2e,
	0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 0xfa, 0xcb, 0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d,
	0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20, 0x11, 0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8,
	0xc5, 0xf4, 0xa7, 0x96, 0x01, 0x30, 0x63, 0x52, 0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb,
	0x3d, 0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71, 0x22, 0x13,
	0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 0xc7, 0xf6, 0xa5, 0x94, 0x03, 0x32, 0x61, 0x50,
	0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c, 0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95,
	0xf8, 0xc9, 0x9a, 0xab, 0x3c, 0x0d, 0x5e, 0x6f, 0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6,
	0x7a, 0x4b, 0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65, 0x54,
	0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 0x80, 0xb1, 0xe2, 0xd3, 0x44, 0x75, 0x26, 0x17,
	0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 0x45, 0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2,
	0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a, 0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91,
	0x47, 0x76, 0x25, 0x14, 0x83, 0xb2, 0xe1, 0xd0, 0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
	0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 0xbd, 0x8c, 0xdf, 0xee, 0x79, 0x48, 0x1b, 0x2a,
	0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49, 0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef,
	0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24, 0x15, 0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac
};

static const  unsigned char B_CRC8_Table[256] = {//鍙嶅簭,浣庝綅鍏堣 x^8+x^5+x^4+1
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

/// 绱㈠紩璧峰鐐癸紝鐩綍鏍�
static cmd_root_t shellcmdroot = {0};

/* Global variables ---------------------------------------------------------*/

/// 榛樿杈撳嚭鏍囧織锛屽彲淇敼
char DEFAULT_INPUTSIGN[COMMANDLINE_MAX_LEN] = "~ # ";

/* Private function prototypes -----------------------------------------------*/
static void   shell_getchar     (struct shell_input * shellin , char ch);
static void   shell_backspace   (struct shell_input * shellin) ;
static void   shell_tab         (struct shell_input * shellin) ;
       void   shell_confirm     (struct shell_input * shellin ,char * info ,cmd_fn_t yestodo);

#if (COMMANDLINE_MAX_RECORD)//濡傛灉瀹氫箟浜嗗巻鍙茬邯褰�
	static char * shell_record(struct shell_input * shellin);
	static void   shell_show_history(struct shell_input * shellin,int LastOrNext);
#else
#	define shell_record(x)
#	define shell_show_history(x,y)
#endif //#if (COMMANDLINE_MAX_RECORD)//濡傛灉瀹氫箟浜嗗巻鍙茬邯褰�

/* Gorgeous Split-line ------------------------------------------------------*/

static struct shellcommand *shell_search_cmd(cmd_root_t * root , unsigned int cmdindex)
{
	struct shellcommand * command ;
	cmd_entry_t *node = ROOT(root);

	#if USE_AVL_TREE 
		while (node) {
			command = container_of(node, struct shellcommand, node);
			if (cmdindex < command->ID)
				node = node->avl_left;
			else 
			if (cmdindex > command->ID)
				node = node->avl_right;
			else 
				return command;
		}
	#else 
		for ( ; node ; node = node->next ) {
			command = container_of(node, struct shellcommand, node);
			if (command->ID > cmdindex)
				return NULL;
			else
			if (command->ID == cmdindex)
				return command;
		}
	#endif 
  
	return NULL;
}



static int shell_insert_cmd(cmd_root_t * root , struct shellcommand * newcmd)
{
	struct shellcommand * command ;
	cmd_entry_t **node = &ROOT(root) ;

	#if USE_AVL_TREE 
		/* 鐢ㄥ钩琛′簩鍙夋爲鏋勫缓鏌ヨ绯荤粺 */
		cmd_entry_t *parent = NULL;

		/* Figure out where to put new node */
		while (*node) {
			command = container_of(*node, struct shellcommand, node);
			parent = *node;
			if (newcmd->ID < command->ID)
				node = &((*node)->avl_left);
			else 
			if (newcmd->ID > command->ID)
				node = &((*node)->avl_right);
			else
				return 1;
		}

		/* Add new node and rebalance tree. */
		avl_insert(root,&newcmd->node,parent,node);
	#else 
		/* 鐢ㄥ崟閾捐〃鏋勫缓鏌ヨ绯荤粺 */
		for(; *node; node = &((*node)->next))
		{
			command = container_of(*node, struct shellcommand, node);
			if (newcmd->ID == command->ID) 
			{
				return -1;
			}
			else if (command->ID > newcmd->ID)
			{
				break ;
			}
		}
		newcmd->node.next = *node; 
		*node = &newcmd->node;
	#endif 
	return 0;
}


static int checkout(char * str,int len,struct shellcommand ** checkout , int checkmax)
{
	unsigned int index , end;
	int      matchnums = 0;
	struct shellcommand * shellcmd = NULL;
	cmd_entry_t  * node = ROOT(&shellcmdroot);

	/* 棣栧瓧姣嶇浉鍚屽苟涓旈暱搴︿笉灏忎簬 len 鐨勭偣浣滀负璧峰鐐癸紝涓嬩竴涓瓧姣嶅紑澶寸殑鐐逛綔涓虹粨鏉熺偣 */
	index = ((unsigned int)(*str)<<26) | (len << 21) ;
	end = (unsigned int)(*str + 1)<<26 ; 

	/* 鍏堟壘鍒拌捣濮嬪尮閰嶇偣 */
	#if USE_AVL_TREE 
		/* index 涓嶅瓨鍦紝鏌ユ壘缁撴潫鍚庣殑 shell_cmd 鏈�闈犺繎 index 鐢ㄦ浣滀负璧峰鍖归厤鐐� */
		while ( node ){
			shellcmd = container_of(node,struct shellcommand, node);	
			node = (index < shellcmd->ID) ? node->avl_left : node->avl_right;
		}

		if (shellcmd)
		   node = &shellcmd->node ;
	#else 
		/* 鏌ユ壘鍒伴瀛楁瘝鐩稿悓鐨勭偣浣滀负璧峰鐐� */
		for ( ; node ; node = NEXT(node)) { 
			shellcmd = container_of(node, struct shellcommand, node);
			if (shellcmd->ID > index)
				break;
		}
	#endif

	for( ; node ; node = NEXT(node) ) {
		/* 瀵规瘮杈撳叆鐨勫瓧绗︿覆锛屽鏋滃墠 len 涓瓧绗︿笌 str 鐩稿悓,鎶婂懡浠ゅ潡璁颁笅鏉� */
		shellcmd = container_of(node,struct shellcommand, node);
		if (shellcmd->ID > end) {
			break ;
		}

		if (0 == memcmp(shellcmd->name, str, len)){ 
			checkout[matchnums] = shellcmd;
			if (++matchnums > 10) {
				return 0;    
			}
		}
	}

	return matchnums;
}


#if (COMMANDLINE_MAX_RECORD) //濡傛灉瀹氫箟浜嗗巻鍙茬邯褰�

static char * shell_record(struct shell_input * shellin)
{	
	char *  history = &shellin->history[shellin->htywrt][0];
	
	shellin->htywrt  = (shellin->htywrt + 1) % COMMANDLINE_MAX_RECORD;
	shellin->htyread = shellin->htywrt;

	memcpy(history,shellin->cmdline,shellin->tail);
	history[shellin->tail] = 0;
	
	return history;
}


static void shell_show_history(struct shell_input * shellin,int LastOrNext)
{
	int len = 0;
	
	printk("\33[2K\r%s",shellin->sign);//"\33[2K\r" 琛ㄧず娓呴櫎褰撳墠琛�

	if (!LastOrNext) //涓婄澶达紝涓婁竴鏉″懡浠�
		shellin->htyread = (!shellin->htyread) ? (COMMANDLINE_MAX_RECORD - 1) : (shellin->htyread - 1);
	else       //涓嬬澶�
	if (shellin->htyread != shellin->htywrt)
		shellin->htyread = (shellin->htyread + 1) % COMMANDLINE_MAX_RECORD;

	if (shellin->htyread != shellin->htywrt){ //鎶婂巻鍙茶褰曡�冨埌鍛戒护琛屽唴瀛�
		for (char * history = &shellin->history[shellin->htyread][0]; *history ; ++len)
			shellin->cmdline[len] = *history++;
	}
	
	shellin->cmdline[len] = 0; //娣诲姞缁撴潫绗�
	shellin->tail = len ;
	shellin->edit = len ;

	if (len) printl(shellin->cmdline,len); //鎵撳嵃鍛戒护琛屽唴瀹�
}

#endif //#if (COMMANDLINE_MAX_RECORD) //濡傛灉瀹氫箟浜嗗巻鍙茬邯褰�



static void shell_tab(struct shell_input * shellin)
{
	struct shellcommand * match[10];  
	char  *  str = shellin->cmdline;
	int  len = shellin->tail;
	int matchnums = 0 ; 
	
	/* Shave off any leading spaces */
	for ( ; *str == ' ' ; ++str) {
		--len; 
	}

	if (*str == 0 || len == 0 ){
		return ;
	}

	/* 濡傛灉娌℃湁鍛戒护鍖呭惈杈撳叆鐨勫瓧绗︿覆锛岃繑鍥� */
	matchnums = checkout(str,len,match,10);
	if (!matchnums){ 
		return ; 
	}

	/* 濡傛灉缂栬緫浣嶇疆涓嶆槸鏈锛屽厛鎶婂厜鏍囩Щ鍒版湯绔� */
	if (shellin->edit != shellin->tail) { 
		printl(&shellin->cmdline[shellin->edit],shellin->tail - shellin->edit);
		shellin->edit = shellin->tail;
	}

	if(1 == matchnums)
	{
		/* 濡傛灉鍙壘鍒颁簡涓�鏉″懡浠ゅ寘鍚綋鍓嶈緭鍏ョ殑瀛楃涓诧紝鐩存帴琛ュ叏鍛戒护骞舵墦鍗� */
		for(char * fmt = match[0]->name + len; *fmt; ++fmt)
		{
			shell_getchar(shellin, *fmt);
		}
		shell_getchar(shellin, ' ');
	}
	else
	{
		/* 濡傛灉涓嶆涓�鏉″懡浠ゅ寘鍚綋鍓嶈緭鍏ョ殑瀛楃涓诧紝鎵撳嵃鍚湁鐩稿悓
		  瀛楃鐨勫懡浠ゅ垪琛紝骞惰ˉ鍏ㄥ瓧绗︿覆杈撳嚭鐩村埌鍛戒护鍖哄垎鐐� */
		for(int i = 0;i < matchnums; ++i)
		{
			printk("\r\n\t%s",match[i]->name); 
		}

		printk("\r\n%s%s",shellin->sign,shellin->cmdline);
		
		while(1U)
		{
			/* 鎶� match[] 涓惈鏈夌浉鍚岀殑瀛楃琛ュ叏鍒拌緭鍏ョ紦鍐蹭腑 */
			for(int i = 1; i < matchnums; ++i)
			{
				if(match[0]->name[len] != match[i]->name[len])
				{
					return;
				}
			}
			shell_getchar(shellin,match[0]->name[len++]);
		}
	}
}


static void shell_backspace(struct shell_input * shellin)
{
	char   printbuf[COMMANDLINE_MAX_LEN*2]={0};//涓浆鍐呭瓨
	char * print = &printbuf[1];
	char * printend = print + (shellin->tail - shellin->edit) + 1;
	char * edit = &shellin->cmdline[shellin->edit--] ;
	char * tail = &shellin->cmdline[shellin->tail--];

	/* 褰撹緭鍏ヨ繃宸﹀彸绠ご鏃讹紝闇�瑕佷綔瀛楃涓叉彃鍏ュ乏绉诲鐞嗭紝骞朵綔鍙嶉鍥炴樉
	   濡� abUcd 涓垹闄锛岄渶瑕佸乏绉籧d锛屽苟鎵撳嵃涓や釜 '\b' 浣垮厜鏍囧洖鍒� ab 澶� */
	for (char * cp = edit - 1 ; edit < tail ; *cp++ = *edit++) {
		*print++ = *edit;
		*printend++ = '\b';
	}

	printbuf[0] = '\b';
	*print = ' ';       //瑕嗙洊鏈�鍚庝竴涓瓧绗︽樉绀�
	*printend++ = '\b'; //鍏夋爣鍥炴樉

	shellin->cmdline[shellin->tail] = 0;  //鏈娣诲姞瀛楃涓茬粨鏉熺
	printl(printbuf,printend-printbuf);
}

static void shell_getchar(struct shell_input * shellin , char ascii)
{
	if (shellin->tail + 1 >= COMMANDLINE_MAX_LEN){
		return ;
	}

	if (shellin->tail == shellin->edit) {
		shellin->cmdline[shellin->edit++] = ascii;
		shellin->cmdline[++shellin->tail] = 0;
		printl(&ascii,1);
	}
	else {
		/* 鍏跺疄 else 鍒嗘敮瀹屽叏鍙互澶勭悊 tail == edit 鐨勬儏鍐� */
		char  printbuf[COMMANDLINE_MAX_LEN*2]={0};
		char *tail = &shellin->cmdline[shellin->tail++];
		char *edit = &shellin->cmdline[shellin->edit++];
		char *print = printbuf + (tail - edit);
		char *printend = print + 1;

		/* 褰撹緭鍏ヨ繃宸﹀彸绠ご鏃讹紝闇�瑕佷綔瀛楃涓叉彃鍏ュ彸绉诲鐞嗭紝骞朵綔鍙嶉鍥炴樉
		   濡� abcd 涓湪bc鎻掑叆U锛岄渶瑕佸彸绉籧d锛屽苟鎵撳嵃涓や釜 '\b' 浣垮厜鏍囧洖鍒� abU 澶� */
		for (char *cp = tail - 1; cp >= edit ; *tail-- = *cp--) {
			*print-- = *cp;
			*printend++ = '\b';
		}

		/* 鎻掑叆瀛楃 */
		*print = ascii; 
		*edit  = ascii;
		shellin->cmdline[shellin->tail] = 0 ;
		printl(printbuf,printend - printbuf);
	}
}



static void shell_parse(cmd_root_t * cmdroot , struct shell_input * shellin)
{
	union uncmd unCmd ;
	unsigned int len = 0;
	unsigned int sum = 0;
	unsigned int fcrc8 = 0;
	unsigned int bcrc8 = 0;
	char  *  str = shellin->cmdline;
	struct shellcommand * cmdmatch;

	/* Shave off any leading spaces */
	for ( ; ' ' == *str ; ++str) ;	

	if (0 == *str)
		goto PARSE_END;

	unCmd.part.FirstChar = *str ;
	for (; (*str) && (*str != ' ') ; ++str ,++len) {
		sum += *str;
		fcrc8 = F_CRC8_Table[fcrc8^*str];
		bcrc8 = B_CRC8_Table[bcrc8^*str];
	}

	unCmd.part.Len = len;
	unCmd.part.Sum = sum;
	unCmd.part.CRC1 = fcrc8;
	unCmd.part.CRC2 = bcrc8;

	cmdmatch = shell_search_cmd(cmdroot,unCmd.ID);
	if (cmdmatch != NULL) {
		/* 鍒ゆ柇鏄惁涓烘湁閫夐」鐨勫懡浠� */
		shellcfm_t * confirm ;
		confirm = container_of(cmdmatch, struct shellconfirm, cmd);
		if (confirm->flag == CONFIRM_FLAG)
		{
			shell_confirm(shellin,confirm->prompt,cmdmatch->func);
		}
		else
		{
			cmdmatch->func(shellin->cmdline);
			printk("\r\n\r\n%s",shellin->sign);
		}
	}
	else
	{
		printk("no reply:%s\r\n\r\n%s",shellin->cmdline, shellin->sign);
	}
	
PARSE_END:
	shellin->tail = 0;//娓呯┖褰撳墠鍛戒护琛岃緭鍏�
	shellin->edit = 0;
	return ;
}

static void shell_clean_screen(void * arg)
{
	printk("\033[2J\033[%d;%dH", 0, 0);
}



static void shell_list_cmd(void * arg)
{
	struct shellcommand * cmd;
	unsigned int firstchar = 0;
	cmd_entry_t  * node ;
	
	for (node = FIRST(&shellcmdroot) ; node; node = NEXT(node))
	{
		cmd = container_of(node,struct shellcommand, node);
		if(firstchar != (cmd->ID & 0xfc000000))
		{
			firstchar = cmd->ID & 0xfc000000;
			printk("\r\n(%c)------",((firstchar>>26)|0x40));
		}
		printk("\r\n\t%s", cmd->name);
	}
}

static void shell_version(void * arg)
{
	printk("\r\n\t%s\r\n",VERSION);
}

static void shell_debug_stream(void * arg)
{
	static const char closemsg[] = "\r\n\tclose debug information stream\r\n\r\n";
	static const char openmsg[] = "\r\n\tget debug information\r\n\r\n";
	int option;
	int argc = cmdline_param(arg, &option, 1);
	
	if ((argc > 0) && (option == 0)) { 
		/* 鍏抽棴璋冭瘯淇℃伅鎵撳嵃娴侊紝浠呮樉绀轰氦浜掍俊鎭� */
		current_puts((char*)closemsg,sizeof(closemsg) - 1);
		default_puts = NULL;  
	}
	else {
		/* 璁剧疆褰撳墠浜や簰涓轰俊鎭祦杈撳嚭 */
		current_puts((char*)openmsg,sizeof(openmsg) - 1);
		default_puts = current_puts;
	}
}

void _shell_register(struct shellcommand * newcmd,char * cmd_name, cmd_fn_t cmd_func)
{
	char * str = cmd_name;
	union uncmd unCmd ;
	unsigned int clen;
	unsigned int fcrc8 = 0;
	unsigned int bcrc8 = 0;
	unsigned int sum = 0;

	for (clen = 0; *str ; ++clen,++str) {
		sum += *str;
		fcrc8 = F_CRC8_Table[fcrc8^*str];
		bcrc8 = B_CRC8_Table[bcrc8^*str];
	}

	unCmd.part.CRC1 = fcrc8;
	unCmd.part.CRC2 = bcrc8;
	unCmd.part.Len = clen;
	unCmd.part.Sum = sum;
	unCmd.part.FirstChar = *cmd_name;
	
	newcmd->ID = unCmd.ID;   //鐢熸垚鍛戒护鐮�
	newcmd->name = cmd_name;
	newcmd->func = cmd_func;
	shell_insert_cmd(&shellcmdroot,newcmd);
}

int cmdline_strtok(char * str ,char ** argv ,int maxread)
{
	int argc = 0;

	for ( ; ' ' == *str ; ++str) ; //璺宠繃绌烘牸
	
	for ( ; *str && argc < maxread; ++argc,++argv ) { //瀛楃涓嶄负 鈥榎0' 鐨勬椂鍊�
	
		for (*argv = str ; ' ' != *str && *str ; ++str);//璁板綍杩欎釜鍙傛暟锛岀劧鍚庤烦杩囬潪绌哄瓧绗�
		
		for ( ; ' ' == *str; *str++ = '\0');//姣忎釜鍙傛暟鍔犲瓧绗︿覆缁撴潫绗︼紝璺宠繃绌烘牸
	}
	
	return argc;
}


int cmdline_param(char * str,int * argv,int maxread)
{
	int argc ;
	unsigned int  value;

	for ( ; ' ' == *str        ; ++str);//璺宠繃绌烘牸
	for ( ; ' ' != *str && *str; ++str);//璺宠繃绗竴涓弬鏁�
	for ( ; ' ' == *str        ; ++str);//璺宠繃绌烘牸

	if (*str == '?')
		return PARAMETER_HELP;//濡傛灉鍛戒护鍚庨潰鐨勬槸闂彿锛岃繑鍥瀐elp

	for (argc = 0; *str && argc < maxread; ++argc , ++argv) { //瀛楃涓嶄负 鈥榎0' 鐨勬椂鍊�
	
		*argv = 0;
		
		if ('0' == str[0] && 'x' == str[1]) { //"0x" 寮�澶达紝鍗佸叚杩涘埗杞崲
			for ( str += 2 ;  ; ++str )  {
				if ( (value = *str - '0') < 10 ) // value 鍏堣祴鍊硷紝鍚庡垽鏂�
					*argv = (*argv << 4)|value;
				else
				if ( (value = *str - 'A') < 6 || (value = *str - 'a') < 6)
					*argv = (*argv << 4) + value + 10;
				else
					break;
			}
		}
		else { //寰幆鎶婂瓧绗︿覆杞负鏁板瓧锛岀洿鍒板瓧绗︿笉涓� 0 - 9
			unsigned int minus = ('-' == *str);//姝ｈ礋鏁拌浆鎹�
			if (minus)
				++str;

			for (value = *str - '0'; value < 10 ; value = *(++str) - '0')
				*argv = (*argv * 10 + value);
			
			if (minus)
				*argv = -(*argv);
		}

		if ('\0' != *str && ' ' != *str)//濡傛灉涓嶆槸 0 - 9 鑰屼笖涓嶆槸绌烘牸锛屽垯鏄敊璇瓧绗�
			return PARAMETER_ERROR;

		for ( ; ' ' == *str ; ++str);//璺宠繃绌烘牸,缁х画鍒ゆ柇涓嬩竴涓弬鏁�
	}

	return argc;
}

void cmdline_gets(struct shell_input * shellin,char * recv,int len)
{
	int state = 0 ;

	for(char * end = recv + len; recv < end; ++recv)
	{
		if (0 == state)
		{
			/* 鏅�氬瓧绗﹁鍏ュ唴瀛�;鍚﹀垯鍒ゆ柇鐗规畩鍔熻兘瀛楃 */
			if((*recv > 0x1F) && (*recv < 0x7f))
			{
				shell_getchar(shellin,*recv);
			}
			else
			{
				switch(*recv)
				{
				case KEYCODE_ENTER:
					if (shellin->tail){
						printk("\r\n");
						shell_record(shellin);
						shell_parse(&shellcmdroot ,shellin);
					}
					else{
						printk("\r\n%s",shellin->sign);
					}
					break;
				case KEYCODE_ESC :
					state = 1;
					break;
				case KEYCODE_CTRL_C:
					shellin->edit = 0;
					shellin->tail = 0;
					printk("^C\r\n%s",shellin->sign);
					break;
				case KEYCODE_BACKSPACE :
				case 0x7f: /* for putty */
					if (shellin->edit)
						shell_backspace(shellin);
					break;
				case KEYCODE_TAB:
					shell_tab(shellin);
					break;
				default: ;
				}
			}
		}
		else if (1 == state)
		{ 
			/* 鍒ゆ柇鏄惁鏄澶村唴瀹� */
			state = (*recv == '[') ? 2 : 0 ;
		}
		else if(2 == state)
		{
			/* if (2 == state) 鍝嶅簲绠ご鍐呭 */
			switch(*recv){  
			case 'A'://涓婄澶�
				shell_show_history(shellin,0);
				break;
			case 'B'://涓嬬澶�
				shell_show_history(shellin,1);
				break;
			case 'C'://鍙崇澶�
				if ( shellin->tail != shellin->edit)
					printl(&shellin->cmdline[shellin->edit++], 1);
				break;
			case 'D'://宸︾澶�
				if (shellin->edit)
				{
					--shellin->edit;
					printl("\b",1);
				}
				break;
			case '1':
				state = 3;
				break;
			case '4':
				state = 4;
				break;
			default:;
			} //switch 绠ご鍐呭
		}// if (2 == state) //鍝嶅簲绠ご鍐呭
		else if(state == 3)		/*鍝嶅簲Home閿�*/
		{
			if(*recv == 0x7E)
			{
				if(shellin->edit != 0)
				{
					printk("\033[%dD", shellin->edit);
					shellin->edit = 0;
				}
			}
		}
		else if(state == 4)	/*鍝嶅簲End閿�*/
		{
			if(*recv == 0x7E)
			{
				unsigned int len = shellin->tail - shellin->edit;
				if(len != 0)
				{
					shellin->edit += len;
					printk("\033[%dC", len);
				}
			}
		}
	} //for ( ; len && *recv; --len,++recv)
	return ;
}

static void confirm_gets(struct shell_input * shellin ,char * buf , int len)
{
	char * option = &shellin->cmdline[COMMANDLINE_MAX_LEN-1];

	if (0 == *option) { //鍏堣緭鍏� [Y/y/N/n] 锛屽叾浠栨寜閿棤鏁�
		if ('Y' == *buf || 'y' == *buf || 'N' == *buf || 'n' == *buf) {
			*option = *buf;
			printl(buf,1);
		}
	}
	else
	if (KEYCODE_BACKSPACE == *buf) { //鍥為��閿�
		printl("\b \b",3);
		*option = 0;
	}
	else
	if ('\r' == *buf || '\n' == *buf) {//鎸夊洖杞︾‘瀹�
		cmd_fn_t yestodo = (cmd_fn_t)shellin->apparg;
 		char opt = *option ; 
		
		*option = 0 ;  //shellin->cmdline[COMMANDLINE_MAX_LEN-1] = 0;
		shellin->gets   = cmdline_gets;//鏁版嵁鍥炲綊涓哄懡浠よ妯″紡
		shellin->apparg = NULL;

		printl("\r\n",2);

		if ( 'Y' == opt || 'y' == opt) 
			yestodo(shellin->cmdline);
		else
			printk("cancel this operation\r\n");
	}
}

void shell_confirm(struct shell_input * shellin ,char * info ,cmd_fn_t yestodo)
{
	printk("%s [Y/N] ",info);
	shellin->gets = confirm_gets;// 鏁版嵁娴佽幏鍙栬嚦 confirm_gets
	shellin->apparg = yestodo;
	shellin->cmdline[COMMANDLINE_MAX_LEN-1] = 0;
}

void shell_start(struct shell_input * shellin, char *welcome_str)
{
	printk(cyan);
	printk("\r\n\r\n%s\r\n", welcome_str);
	printk("\r\n\r\n%s",shellin->sign);
}

void shell_input_init(struct shell_input * shellin , fmt_puts_t shellputs,...)
{
	unsigned int arg  ;
	char * shellsign = DEFAULT_INPUTSIGN;
	shellgets_t shellgets = cmdline_gets;
	
	va_list ap;
	va_start(ap, shellputs); //妫�娴嬫湁鏃犳柊瀹氫箟

	arg = va_arg(ap, unsigned int) ;
	for (; MODIFY_MASK == (arg & (~0x0f)) ; arg = va_arg(ap, unsigned int) ) {
		if (MODIFY_SIGN == arg) //濡傛灉閲嶅畾涔夊綋鍓嶄氦浜掔殑杈撳叆鏍囧織
			shellsign = va_arg(ap, char*);
		else
		if (MODIFY_GETS == arg) //濡傛灉閲嶅畾涔夊綋鍓嶄氦浜掔殑杈撳叆娴佸悜
			shellgets = (shellgets_t)va_arg(ap, void*);
	}

	va_end(ap);

	shellin->tail = 0;
	shellin->edit = 0;
	shellin->puts = shellputs;
	shellin->gets = shellgets;
	shellin->htywrt  = 0;
	shellin->htyread = 0;
	shellin->apparg  = NULL;
	strcpy(shellin->sign, shellsign);
}


void shell_init(char * defaultsign ,fmt_puts_t puts)
{
	if (defaultsign)
		strncpy(DEFAULT_INPUTSIGN,defaultsign,COMMANDLINE_MAX_LEN);
	//strcpy(DEFAULT_INPUTSIGN,defaultsign);

	current_puts = puts ;
	default_puts = puts ;
	
	/* 娉ㄥ唽涓�浜涘熀鏈懡浠� */
	shell_register_command("cmd-list"     ,shell_list_cmd);
	shell_register_command("shell-version",shell_version);
	shell_register_command("clear"        ,shell_clean_screen);
	shell_register_command("debug-info"   ,shell_debug_stream);
}
