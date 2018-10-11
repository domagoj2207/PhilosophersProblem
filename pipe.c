#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

#define MAXREAD 18/* najveća duljina poruke*/
#define MAXFILOZOFA 10
#define MAXOBROKA 10

int max_filozofa;
int max_obroka;

char recBuf[MAXREAD] = "";	// poruka koja se prima
char tempBuf[MAXREAD] = "";	// pomocno polje
char message[MAXREAD] = ""; /* poruka koja se salje */

int pfd[MAXFILOZOFA][MAXFILOZOFA][2]; // opisnici za cjevovode

int updateClock(int x, int y) {
	int rez;
	if (x > y)
		rez = x + 1;
	else
		rez = y + 1;

	return rez;
}

 void printSpaces(int rank) {
    int i;
    for(i=0; i<=rank; i++) {
        if(i != 0)
            printf("\t\t\t\t\t");
    }
}

void filozof(int myId, int T) {

	int requests[max_filozofa];
	int otherId, otherClock;
	int currentClock = T;
	int newClock = currentClock;

	int i, brojac=0;

	// inicijalizacija
	for(i = 0; i<max_filozofa; i++) {

		requests[i] = 0;

        if (myId != i) {
            close(pfd[myId][i][0]);
			close(pfd[i][myId][1]);
        }
    }
    
    while(brojac < max_obroka) {

		// Sudjelujem na konferenciji, sleep(1)
		printSpaces(myId);
		printf("F%d: Sudjelujem na konferenciji\n", myId);
		sleep(1);

		// Posalji zahtjeve za kriticnim odsjeckom
		sprintf(message, "Zahtjev(P=%d,T=%2d)", myId, currentClock);
		requests[myId] = currentClock;
		for(i = 0; i<max_filozofa; i++) {
			if (i != myId) {
				printSpaces(myId);
				printf("F%d: šaljem F%d-->%s\n", myId, i, message);
				write(pfd[myId][i][1], message, strlen(message) + 1);
				sleep(1);
			}
		}
		
		sleep(1);
		// Primi zahtjeve, azuriraj sat, odgovori ako trebas ili zabiljezi za kasnije
		for (i = 0; i < max_filozofa; i++) {
	        if (i != myId) {
	        	sleep(1);

		        read(pfd[i][myId][0], recBuf, MAXREAD);

		        for(int j=0; j<MAXREAD; j++)
		        	tempBuf[j] = recBuf[j];

		        sscanf(tempBuf, "%*[^0123456789]%d%*[^0123456789]%d%*[^0123456789]", &otherId, &otherClock);
	    		
		        printSpaces(myId);
	    		printf("F%d: primam-->%s\n", myId, recBuf);

				newClock = updateClock(newClock, otherClock);

		        if (requests[myId] != 0 && ((currentClock == otherClock && myId < i) || (currentClock < otherClock))) {
		            requests[i] = otherClock;
		        } 
		        else {
		            sprintf(message, "Odgovor(P=%d,T=%2d)", myId, otherClock);
		            printSpaces(myId);
	    			printf("F%d: šaljem F%d-->%s\n", myId, i, message);
					write(pfd[myId][i][1], message, strlen(message));
		        }
		    }
		    
		}

		// Cekaj na odgovore ostalih procesa filozofa
		for (i = 0; i <max_filozofa; i++) {
	        if (i != myId) {
	        	sleep(3);

	            read(pfd[i][myId][0], recBuf, MAXREAD);
	            
	            for(int j=0; j<strlen(recBuf); j++)
		        	tempBuf[j] = recBuf[j];
		        
	    		sscanf(tempBuf, "%*[^0123456789]%d%*[^0123456789]%d%*[^0123456789]", &otherId, &otherClock);
	    		
	    		printSpaces(myId);
	    		printf("F%d: primam-->%s\n", myId, recBuf);
				newClock = updateClock(newClock, otherClock);
	        }
		}

		
		// Ulazak u K.O. ----> jedem
		printf("\n");
		printSpaces(myId);
		printf("FILOZOF %d JE ZA STOLOM\n\n\n", myId);
		sleep(3);

		// Izlazak iz K.O. ---> odgovori na postojeće zahtjeve
		for (i = 0; i < max_filozofa; i++) {
	        if (i != myId) {
	        	if (requests[i] != 0) {
	          	 	sprintf(message, "Odgovor(P=%d,T=%2d)", myId, requests[i]);
	          	 	printSpaces(myId);
	          	 	printf("F%d: gotov KO šaljem F%d-->%s\n", myId, i, message);
					write(pfd[myId][i][1], message, strlen(message));
	            	requests[i] = 0;
	       		}
	       	}
		}

		currentClock = newClock;
		brojac++;

		sleep(1);
	}

}
void retreat(int failure) {
    exit(0);
}

int main(void) {

	int i, j;
	char clock;
	srand(time(NULL));

	int pid = getpid();

	printf("\n Unesite koliko želite FILOZOFA (min 3, max %d): ", MAXFILOZOFA);
	scanf("%d", &max_filozofa);
	
	if(max_filozofa <= 2 || max_filozofa > MAXFILOZOFA) {
		printf("\n Zadali ste nedozvoljeni broj filozofa! \n\n Gasim program!");
		return(0);
	}

	printf("\n Unesite koliko želite obroka (min 1, max %d): ", MAXOBROKA);
	scanf("%d", &max_obroka);

	if(max_obroka < 1 || max_obroka > MAXOBROKA) {
		printf("\n Zadali ste nedozvoljeni broj obroka! \n\n Gasim program!");
		return(0);
	}

	// inicijaliziraj cjevovode
	for (i = 0; i < max_filozofa; i++) {
        for (j = 0; j < max_filozofa; j++) {
            if(i != j) {
                pipe(pfd[i][j]);
            }
        }
	}

	// stvori procese filozofe
	for (i = 0; i < max_filozofa; i++) {

		switch (fork()) {
			case -1:
				printf("Ne mogu kreirati novi proces\n");
				break;
			case 0:
				for(j=0; j<i+5; j++)
					clock = rand() % 10 + 1;
				
				printf("stvoren FILOZOF %d/%d\n", i, max_filozofa);
				sleep(3);
				filozof(i, clock);	// pozovi funkciju filozof s odgovarajucim parametrima
				break;	
		}
		usleep(1500);

		if(getpid() != pid)
			break;
	}

	if(getpid() == pid) {
		printf("\n");
		sigset(SIGINT, retreat);
		j = 0;
		while (j < max_filozofa) {
        	wait(NULL);
            j++;
        }

        printf("\n\n   Kraj programa\n\n");
	}
	
	exit(0); /* zatvara sve deskriptore */
}