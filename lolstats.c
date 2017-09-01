#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define FILEPATH 1

struct gamestats {
	char gametype[30];
	char map[30];
	int result;
	int duration;
	char date[12];
	char champion[15];
	int kills;
	int deaths;
	int assists;
	int cs;
	int gold;
	int level;
	char summoner1[15];
	char summoner2[15];
};

int CompareString(char *string1, char *string2) {
	int i = 0;
	while (string1[i] == string2[i]) {
		if (string1[i] == 0) {
			return 1;
		}
		i++;
	}
	return 0;
}

int StringToInteger(char *string, int ptr) {
	int integer = 0;
	int digit = 0;
	int scale = 1;
	do {
		digit = string[ptr] - 48;
		integer += digit * scale;
		ptr--;
		scale *= 10;
	} while (string[ptr] > 47 && string[ptr] < 58 && ptr >= 0);
	
	return integer;
}

int searchfor(char *string, char *data1) {
	int i = 0;
	int moved = 0;
	while(string[i] != 0 && *data1 != 0) {
		if (string[i] >= 48 && string[i] <= 57) {
			i++;
			while (*data1 >= 48 && *data1 <= 57) {
				data1++;
				moved++;
			}
		}
		else if (string[i] == *data1) {
			i++;
			data1++;
			moved++;
		}
		else  {
			i = 0;
			data1++;
			moved++;
		}
	}
	
	if (string[i] == 0) {
		return moved;
	}
	
	return 0;
}

int main(int argc, char *argv[]) {
	FILE *data;
	int filesize = 0;
	char *datastr;
	
	if (argc > 1) {
		data = fopen(argv[FILEPATH], "r");
		if (data == NULL) {
			fprintf(stderr, "Can't open file: Error: %s\n", strerror(errno));
			exit(2);
		}
	}
	else { 
		fprintf(stderr, "No file input\n");
		exit(1);
	}
	
	printf("Opened %s\n", argv[FILEPATH]);

	fseek(data, 0, SEEK_END);
	filesize = ftell(data);
	fseek(data, 0, SEEK_SET);
	
	printf("Loading %d bytes of data\n", filesize);
	
	datastr = (char *) malloc(filesize + 1);
	if (datastr == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(3);
	}
	
	fread (datastr, filesize, 1, data);

	datastr[filesize] = 0;
	fclose(data);
	printf("Loaded Successfully\n");
	
	int searchoffset = 0;
	int gamecount = 0;
	int addoffset = 0;
	while((addoffset = searchfor("game-summary-0-container", datastr + searchoffset)) != 0) {
		searchoffset += addoffset;
		gamecount++;
	}
	
	printf("Found %d games\n", gamecount);
	printf("Analysing data...\n\n");
	
	struct gamestats *game;
	game = (struct gamestats *) malloc (gamecount * sizeof(*game));
	
	if (game == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		exit(3);
	}
	
	searchoffset = 0;
	int charc = 0;
	
	for(int i = 0; i < gamecount; i++) { //collect the gamestats from the string
		addoffset = searchfor("game-summary-0-container", datastr + searchoffset);
		searchoffset += addoffset;
		addoffset = searchfor("class=\"champion-nameplate-level\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) >= 48 && *(datastr + searchoffset) <= 57  && *(datastr + searchoffset) != 0);
			searchoffset--;
			game[i].level = StringToInteger(datastr, searchoffset);
		}
		
		addoffset = searchfor("class=\"spell-icon binding\"", datastr + searchoffset);
		searchoffset += addoffset;
		addoffset = searchfor("data-rg-id=\"Summoner", datastr + searchoffset);
		searchoffset += addoffset;
		charc = 0;
		if(addoffset) {
			while (*(datastr + searchoffset) != '"' && *(datastr + searchoffset) != 0){
				game[i].summoner1[charc] = *(datastr + searchoffset);
				charc++;
				searchoffset++;
			}
		}
		game[i].summoner1[charc] = 0;
		
		addoffset = searchfor("class=\"spell-icon binding\"", datastr + searchoffset);
		searchoffset += addoffset;
		addoffset = searchfor("data-rg-id=\"Summoner", datastr + searchoffset);
		searchoffset += addoffset;
		charc = 0;
		if(addoffset) {
			while (*(datastr + searchoffset) != '"' && *(datastr + searchoffset) != 0){
				game[i].summoner2[charc] = *(datastr + searchoffset);
				charc++;
				searchoffset++;
			}
		}
		game[i].summoner2[charc] = 0;
		
		addoffset = searchfor("class=\"champion-nameplate-name\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			charc = 0;
			searchoffset++;
			while (*(datastr + searchoffset) != '<' && *(datastr + searchoffset) != 0){
				game[i].champion[charc] = *(datastr + searchoffset);
				charc++;
				searchoffset++;
			}
		}
		game[i].champion[charc] = 0;

		addoffset = searchfor("class=\"map-mode-mode\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			charc = 0;
			searchoffset++;
			while (*(datastr + searchoffset) != '<' && *(datastr + searchoffset) != 0){
				game[i].map[charc] = *(datastr + searchoffset);
				charc++;
				searchoffset++;
			}
		}
		game[i].map[charc] = 0;
		
		addoffset = searchfor("class=\"map-mode-queue\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			charc = 0;
			searchoffset++;
			while (*(datastr + searchoffset) != '<' && *(datastr + searchoffset) != 0){
				game[i].gametype[charc] = *(datastr + searchoffset);
				charc++;
				searchoffset++;
			}
		}
		game[i].gametype[charc] = 0;
		
		addoffset = searchfor("class=\"kda-plate-kda\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) >= 48 && *(datastr + searchoffset) <= 57  && *(datastr + searchoffset) != 0);
			searchoffset--;
			game[i].kills = StringToInteger(datastr, searchoffset);
			
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) >= 48 && *(datastr + searchoffset) <= 57  && *(datastr + searchoffset) != 0);
			searchoffset--;
			game[i].deaths = StringToInteger(datastr, searchoffset);
			
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) >= 48 && *(datastr + searchoffset) <= 57  && *(datastr + searchoffset) != 0);
			searchoffset--;
			game[i].assists = StringToInteger(datastr, searchoffset);
		}
		
		addoffset = searchfor("class=\"income-minions\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) >= 48 && *(datastr + searchoffset) <= 57  && *(datastr + searchoffset) != 0);
			searchoffset--;
			game[i].cs = StringToInteger(datastr, searchoffset);
		}
		
		addoffset = searchfor("class=\"income-gold\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) >= 48 && *(datastr + searchoffset) <= 57  && *(datastr + searchoffset) != 0);
			searchoffset--;
			game[i].gold = StringToInteger(datastr, searchoffset) * 1000;
			searchoffset += 2;
			game[i].gold += StringToInteger(datastr, searchoffset) * 100;
		}
		
		addoffset = searchfor("<span class=\"date-duration-duration\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			do searchoffset++;
			while (*(datastr + searchoffset) >= 48 && *(datastr + searchoffset) <= 57  && *(datastr + searchoffset) != 0);
			searchoffset--;
			game[i].duration = StringToInteger(datastr, searchoffset) * 60;
			searchoffset += 3;
			game[i].duration += StringToInteger(datastr, searchoffset);
		}
		
		addoffset = searchfor("class=\"date-duration-date\">", datastr + searchoffset);
		searchoffset += addoffset;
		if(addoffset) {
			do searchoffset++;
			while (*(datastr + searchoffset) != '>' && *(datastr + searchoffset) != 0);
			charc = 0;
			searchoffset++;
			while (*(datastr + searchoffset) != '<' && *(datastr + searchoffset) != 0){
				game[i].date[charc] = *(datastr + searchoffset);
				charc++;
				searchoffset++;
			}
		}
		game[i].date[charc] = 0;
		
		addoffset = searchfor("id=\"result-marker-775\" class=\"game-summary-", datastr + searchoffset);
		searchoffset += addoffset;
		if(*(datastr + searchoffset) == 'v') {
			game[i].result = 1; 
		}
		else game[i].result = 0;
	}
	
	free(datastr);
	
	
	
	char champion[50][15];
	int wincount[50];
	int count[50];
	float winrate[50];
	int a = 0, b = 0, c = 0, counttotal = 0;
	
	for(int i = 0; i < 50; i++) {
		for(int j = 0; j < 15; j++) champion[i][j] = 0;
		count[i] = 0;
		wincount[i] = 0;
	}
	
	for(int i = 0; i < gamecount; i++) { //check wins for champions
		a = 0;
		b = 0;
		c = 0;

		if (CompareString("Summoner's Rift", game[i].map)) {
			if (CompareString("Normal (Draft Mode)", game[i].gametype)) c = 1;
			if (CompareString("Ranked (Draft Mode)", game[i].gametype)) c = 1;
			if (CompareString("Normal (Blind Pick)", game[i].gametype)) c = 1;
		}
		if (c) {
			while (a < counttotal) {
				if (CompareString(champion[a], game[i].champion)) {
					break;
				}
				a++;
			}
			if (!(a < counttotal) && counttotal < 50) {
				while (game[i].champion[b]) {
					champion[a][b] = game[i].champion[b];
					b++;
				}
				counttotal++;
			}
			if (game[i].result){
			wincount[a]++;
			count[a]++;
			} else count[a]++;
			
		}
	}
	
	for (int i = 0; i < counttotal; i++){
		winrate[i] = (((float)wincount[i]/(float)count[i]) * 100.00);
		/*printf("%15s:  ", champion[i]);
		printf("%.2f  ", winrate[i]);
		printf("%d\n", count[i]);*/
	}
	
	char ochampion[counttotal][15];
	int ocount[counttotal];
	float owinrate[counttotal];
	float top = 0, ceiling;
	int oi = 0, topi, foundone;
	
	for(int i = 0; i < counttotal; i++) {
		for(int j = 0; j < 15; j++) ochampion[i][j] = 0;
		ocount[i] = 0;
		owinrate[i] = 0;
	}
	
	for (int i = 0; i < counttotal; i++) {
		if (winrate[i] > top) top = winrate[i];
	} 
	ceiling = top + 0.01;
	
	while (oi < counttotal) {
		top = -1;
		topi = 0;
		for (int i = 0; i < counttotal; i++) {
			if (winrate[i] > top && ceiling > winrate[i]) {
				top = winrate[i];
				topi = i;
			}
		}
		for (int j = 0; j < 15; j++) ochampion[oi][j] = champion[topi][j];
		ocount[oi] = count[topi];
		owinrate[oi] = winrate[topi];
		ceiling = top;
		oi++;
		foundone = 1;
		while(foundone) {
			if ((topi + 1) >= counttotal) foundone = 0;
			for (int i = (topi + 1); i < counttotal; i) {
				if (winrate[i] == top) {
					topi = i;
					for (int j = 0; j < 15; j++) ochampion[oi][j] = champion[topi][j];
					ocount[oi] = count[topi];
					owinrate[oi] = winrate[topi];
					oi++;
					i++;
					foundone = 1;
					break;
				}
				else {
					i++;
					foundone = 0;
				} 
			}
		} 
	} 
	
	for (int i = 0; i < counttotal; i++){
		printf("%15s:  ", ochampion[i]);
		printf("%3d  ", ocount[i]);
		printf("%.2f\n", owinrate[i]);
	}

	free(game);
}