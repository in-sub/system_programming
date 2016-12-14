/*crossyRoad.c*/
#include	<stdio.h>
#include	<curses.h>
#include	<signal.h>
#include	<string.h>
#include	<termios.h>
#include	<stdlib.h>
#include	<pthread.h>
#include	<unistd.h>
#include	<time.h>
#include	<sys/time.h>
#include	"crossyRoad.h"

struct ppball 	the_ball;
struct car_t	the_cars[CAR_NUM];
int		game_over = 0;
int		score = 0;
int 		c = 0;

void set_up();		//전체 초기화
void init_ball();	//공 초기화
void init_car();	//차 초기화
void add_boundary();	//경계선  그리기
void add_road();	//도로 그리기
void ball_move(void);	//공 움직임
void ball_in_boundary(struct ppball*);	//공이 경계선을 넘지 않도록
void set_score(int);
void *car_thread(void *);
void boucne_car(struct car_t*);
void *input_char(void *);
int set_ticker(int);

int main(){
	int c, i;
	pthread_t ct, rt, it;

	void *car_thread(void *);

	set_up();

	pthread_create(&ct, NULL, car_thread, (void *)NULL);
	pthread_create(&it, NULL, input_char, (void *)NULL);

	do{
		i = (Y_INIT-2-the_ball.y_pos)/2;
	}while( the_cars[i].x_pos != the_ball.x_pos && score <= MAX_SCORE && c != 'Q') ;
	
	if(score == MAX_SCORE)
		printf("you win!!");
	else{
		game_over = 1;
		printf("game over\n");
	}
	sleep(10);

	pthread_join(ct, NULL);
	pthread_join(it, NULL);

	endwin();
}

void set_up(){
	init_ball();							//공 초기화
	init_car();

	initscr();
	noecho();
	crmode();

	signal(SIGINT, SIG_IGN);
	mvaddstr(5, 9, "SCORE: ");
	mvaddstr(5, 16, "0");
	add_boundary();							//경계선 그리기
	add_road();							//도로 그리기
	mvaddch(the_ball.y_pos, the_ball.x_pos, the_ball.symbol);	//공 그리기
	refresh();

}

void init_ball(){
	the_ball.y_pos = Y_INIT;
        the_ball.x_pos = X_INIT;
        the_ball.y_dir = 0;
        the_ball.x_dir = 0;
        the_ball.symbol = BALL_SYMBOL;
}

void init_car(){	

	srand((unsigned)time(NULL));

	for(int i = 0; i < CAR_NUM; i++){
		the_cars[i].idx = i;
		the_cars[i].y_pos = Y_INIT-2*(i+1);
		the_cars[i].speed = rand()%5+1;

		if(the_cars[i].speed % 2 == 1){
			 the_cars[i].dir = 1;
			 the_cars[i].x_pos = LEFT_EDGE+1;
		}else{
			the_cars[i].dir = -1;
			the_cars[i].x_pos = RIGHT_EDGE-1;
		}
		the_cars[i].symbol = CAR_SYMBOL;
		mvaddch(the_cars[i].y_pos, the_cars[i].x_pos, the_cars[i].symbol);
		refresh();
	}
}

void add_boundary(){
	int i;

	//상
	for(i=LEFT_EDGE+1; i<RIGHT_EDGE; i+=2)	mvaddch(TOP_ROW, i, BNDR_SYMBOL);
	//하
	for(i=LEFT_EDGE+1; i<RIGHT_EDGE; i+=2)	mvaddch(BOT_ROW, i, BNDR_SYMBOL);
	//좌
	for(i=TOP_ROW+1; i<BOT_ROW; i++)	mvaddch(i, LEFT_EDGE, BNDR_SYMBOL);
	//우
	for(i=TOP_ROW+1; i<BOT_ROW; i++)	mvaddch(i, RIGHT_EDGE, BNDR_SYMBOL);
}

void add_road(){
	int i;

	for(i=11; i<20; i+=2)	mvaddstr(i, 10, ROAD_SYMBOL);
}

void ball_move(){
	int y_cur, x_cur;
/*	static int back_cnt = 0;
	static int score = 0;
	char str_score[3];*/

	y_cur = the_ball.y_pos;
	x_cur = the_ball.x_pos;

	ball_in_boundary(&the_ball);
/*
	if(y_cur < the_ball.y_pos)
		back_cnt++;
	else if(y_cur > the_ball.y_pos){
		if(back_cnt > 0)
			back_cnt--;
		else if(back_cnt == 0){
			score++;
			sprintf(str_score, "%d", score);
			mvaddstr(5, 16, "   ");
			mvaddstr(5, 16, str_score);
		}
	}*/

	set_score(y_cur);	

	mvaddch(y_cur, x_cur, BLANK);
	mvaddch(the_ball.y_pos, the_ball.x_pos, the_ball.symbol);
	move(0, 0);
	refresh();
}

void ball_in_boundary(struct ppball *bp){
	int y, x;
	y = bp->y_pos + bp->y_dir;
	x = bp->x_pos + bp->x_dir;

	if(y>TOP_ROW && y<BOT_ROW && x>LEFT_EDGE && x<RIGHT_EDGE){
		bp->y_pos = y;
		bp->x_pos = x;
	}
}

void set_score(int y_cur){
	static int back_cnt = 0;
        char str_score[3];

	if(y_cur < the_ball.y_pos)
                back_cnt++;
        else if(y_cur > the_ball.y_pos){
                if(back_cnt > 0)
                        back_cnt--;
                else if(back_cnt == 0){
                        score++;
                        sprintf(str_score, "%d", score);
                        mvaddstr(5, 16, "   ");
                        mvaddstr(5, 16, str_score);
                }
        }

}

void *car_thread(void *m){

	int *i = (int*) m;
	
	void car_move(int);
	signal(SIGALRM, car_move);
	set_ticker( 1000 );

	while(c != 'Q' && game_over != 1 && score < MAX_SCORE);
		return;
}

void car_move(int signum){
	int x_cur, y_cur;
	int i;

	for(i = 0; i < CAR_NUM; i++){
		struct car_t *cp = &the_cars[i];

		y_cur = cp->y_pos;
		x_cur = cp->x_pos;
	
		bounce_car(cp);

		mvaddch(y_cur, x_cur, BLANK);
		mvaddch(cp->y_pos, cp->x_pos, cp->symbol);
		move(0, 0);
	}
	refresh();
}


void bounce_car(struct car_t *cp) {
        int x;
        x = cp->x_pos + cp->dir * cp->speed;

        if( x < LEFT_EDGE && cp->dir == -1)
		cp->dir = 1; 
	else if( x > RIGHT_EDGE && cp->dir == 1)
                cp->dir = -1;
	else
		cp->x_pos = x;
}

void *input_char(void *m){

	  while((c=getchar()) !='Q' && game_over != 1 && score < MAX_SCORE){
	  	if(c=='w')      {the_ball.y_dir = -2;   the_ball.x_dir = 0;}
		else if(c=='s') {the_ball.y_dir = 2;    the_ball.x_dir = 0;}
		else if(c=='a') {the_ball.x_dir = -1;   the_ball.y_dir = 0;}
		else if(c=='d') {the_ball.x_dir = 1;    the_ball.y_dir = 0;}
		ball_move();
	}
}

int set_ticker(int n_msecs){
        struct itimerval new_timeset;
        long n_sec, n_usecs;

        n_sec = n_msecs/1000;
        n_usecs = (n_msecs%1000) * 1000L;

        new_timeset.it_interval.tv_sec = n_sec;
        new_timeset.it_interval.tv_usec = n_usecs;
        new_timeset.it_value.tv_sec = n_sec;
        new_timeset.it_value.tv_usec = n_usecs;

        return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

