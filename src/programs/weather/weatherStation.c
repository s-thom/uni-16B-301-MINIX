#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <minix/subserve.h>
#include <string.h>
#include <time.h>

/* weather station will generate weather information and put it onto a channel */

/* weather station takes two arguments main(int updateIntervil, int timeToRun) if time to run is not set the program will run until the computer is shutdown */

int ranGen(int max){
  int r = 0;
  r = rand() % max;
  return r;
}

typedef struct weather WEATHER;

struct weather{
  char discription[8];
  int windspeed;
  char wind_direction[2];
  int temp;
  int humidity;
  struct tm timeOfPush;
  
};

struct tm getTime(void){
  time_t rawtime;
  struct tm * timeinfo;
  
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return *timeinfo;
  
}

/* in a reall word case this would poll a webserver or sensors but in this case it will be randomy generated as a prof of consept */
WEATHER *setWeather(void){
  char *weather_discription_array[3] = {"sunny", "cloudy", "raining"};
  char *wind_direction_array[8] = {"N","NE","E","SE", "S", "SW", "W", "NW"};

  WEATHER *w = (WEATHER *) malloc(sizeof(WEATHER));
  strcpy(w->discription, weather_discription_array[ranGen(3)]);
  strcpy(w->wind_direction, wind_direction_array[ranGen(8)]);
  w->temp = (ranGen(55) - 15);
  w->windspeed = ranGen(100);
  w->humidity = ranGen(100);
  w->timeOfPush = getTime();
  
  /* printf ( "Current local time and date: %s", asctime(getTime())); */
  

  return w;
}

int push_loop(int intival, int cycles){
  
  WEATHER *w;

  while(cycles != 0){
    sleep(intival);
    
    w = setWeather();
    /* printf("Tempture = %d\n", w->temp); */
    
    if(!push("localWeather", w, sizeof(WEATHER))){
      printf("weather station failed to push to server :(\nclosing weather station\n");
      break;
    }

    free(w);

    if(cycles > 0) cycles--;
  }
  printf("Weather Station has timed out and is now about to shutdown\n");
}

int main(int argc, char **argv){
  
  
  /* values set to default so update every 5 secounds and run forever */     
  int intervil = 5;
  int timeToRun = -1;
  
  srand(time(NULL));  
  
  if(argc > 1) intervil = atoi(argv[1]);
  if(argc == 3) timeToRun = atoi(argv[2]);

  if(create_channel("localWeather", sizeof(WEATHER))){
    printf("Weather is now being pushed to channel\n");    
    push_loop(intervil, timeToRun);
    close_channel("localWeather");
  }
  else{
    printf("channel could not be created at this time, please try again later\n");
    return 0;  
  }  
}
