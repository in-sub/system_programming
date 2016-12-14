/*crossyRoad.h*/

#define BLANK		' '
#define BALL_SYMBOL	'O'
#define ROAD_SYMBOL	"========================="
#define BNDR_SYMBOL	'+'				//경계선 심볼
#define CAR_SYMBOL	'X'
#define TOP_ROW		9
#define BOT_ROW		21
#define LEFT_EDGE	9
#define	RIGHT_EDGE	35

#define X_INIT		22
#define Y_INIT		20
#define CAR_NUM		4
#define TICKS_PER_SEC	50

struct ppball{
		int y_pos, x_pos;
		int y_dir, x_dir;
		char symbol;
	};

struct car_t{
		int idx;
		int y_pos, x_pos;
		int speed;
		int dir;
		char symbol;
	};
