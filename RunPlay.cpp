#include "football.h"

extern Team *A, *B;
extern int score[2], time_left, play;

extern int *fg, *att, *pick, *sack, *miss, *td, *swat, *cth, *passyrds, *punt;
extern enum gamestate state;


int RunPlay(){

	Team *O, *D; //Pointers to offence and defense

	if(state == Aoff){ 	// if A is on offense
		O = A;
		D = B;
	} else {		//if B is on offense
		O = B;
		D = A;
	}


	int yrds, thrw;

	cout << "SELECTING PLAY " << play << endl;

	switch(play){

		case PUNT:	
			cout << "Team punts" << endl;
			pos = pos + Punt(D->kr, O->p);
			punt++;

			//If the ball is kicked into the endzome then it is a touchback
			//and the other team gets it at the 20 yard line
			if(pos>99){
				cout << "Touchback" << endl;
				pos=80;
			}
			pos = 100-pos;
			state = Boff;
			return 1;
			break;
		case FIELDGOAL:
			if(Fieldgoal(O->k, (100-pos+17))){
				cout << "Fieldgoal is good!" << endl;
				state = Akick;
				*fg++;
				score[0] += 3;
			} else {
				cout << "Fieldgoal is no good!" << endl;
				pos = 100-pos;
				Turnover();
			}
			return 1;
			break;

		case DEEPPASS:
			//determines how many yards the pass will gain IF completeld
			yrds = PassYards(play, O->qb, O->wr1, D->cb1);

			//the time_left will go down as plays happen, this was just a simple temporary fix, will put
			//a more detailed function in later
			time_left -= (yrds/TIME);

			//if this is true then the QB was sacked. Team A loses 5 yards and the down
			if(Blitz(O->rde, D->lt) || Blitz(O->rde, D->lt) || Blitz(O->rde, D->lt)){
				*sack++;
				pos -= 5;

			}else{
				//This determines the outcome of the pass, a 0 means the qb missed or the receiver 
				//dropped the ball, and 1 means the defender swatted the pass away, a 2 means the 
				//defender intercepted the pass which means team B is then on offence, and a 3 means 
				//team A's receiver successfully caught the ball
				thrw=Pass(O->qb, O->wr1, D->cb1, yrds, time_left);
				*att++;

				//miss or dropped
				if(thrw==0){
					*miss++;
					cout << "Incomplete!" << endl;
				}

				//pass swatted
				else if(thrw==1){
					*swat++;
					cout << "Pass defended!" << endl;
				}

				//pass intercepted
				else if(thrw==2){
					*pick++;
					down=0;
					tofirst=10;
					state = Boff;
					pos = 100-pos-yrds;
					if(pos<1){
						//If the ball is intercepted in the endzome then it is a
						//touchback and the other team gets it at the 20 yard line
						cout << "Touchback" << endl;
						pos=20;
					}
					cout << "Interception!" << endl;
				}

				//pass caught
				else if(thrw==3){
					*cth++;
					passyrds += yrds;
					cout << "Pass completed for " << yrds << " yards!" << endl;
					pos = pos + yrds;
				}

			}
			break;


	}
	return 1;
}
