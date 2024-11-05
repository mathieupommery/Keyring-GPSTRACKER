//
// Created by sztuka on 22.01.2023.
//
#include "nmea_parse.h"
#include "math.h"
extern double * distanceparcouru;

//code globalement recuperer, nous nous en somme inspiré afin de recuperer la vitesse sur la trame gnrmc, ce code se base sur des strtok afin de segmenter les trames puis sur des strtol
//ou strtof afin d'en recuperer les informations et de les stocker dans la structure cf nmea_parsE.h
//on a une fonction de decodage par typme de trame interressante, puis une fonction nmea_parse servant à mettre à jour la structure de donnée avec lesdonnées presentes dans le databuffer, qui lui se met à jour tout seul.

char *data[15];

int gps_checksum(char *nmea_data)
{
    //if you point a string with less than 5 characters the function will read outside of scope and crash the mcu.
    if(strlen(nmea_data) < 5) return 0;
    char recv_crc[2];
    recv_crc[0] = nmea_data[strlen(nmea_data) - 4];
    recv_crc[1] = nmea_data[strlen(nmea_data) - 3];
    int crc = 0;
    int i;

    //exclude the CRLF plus CRC with an * from the end
    for (i = 0; i < strlen(nmea_data) - 5; i ++) {
        crc ^= nmea_data[i];
    }
    int receivedHash = strtol(recv_crc, NULL, 16);
    if (crc == receivedHash) {
        return 1;
    }
    else{
        return 0;
    }
}

int nmea_GPGGA(GPS *gps_data, char*inputString){
    char *values[25];
    int counter = 0;
    memset(values, 0, sizeof(values));
    char *marker = strtok(inputString, ",");
    while (marker != NULL) {
        values[counter++] = malloc(strlen(marker) + 1); //free later!!!!!!
        strcpy(values[counter - 1], marker);
        marker = strtok(NULL, ",");
    }
    char lonSide = values[5][0];
    char latSide = values[3][0];
    strcpy(gps_data->lastMeasure, values[1]);
    if(latSide == 'S' || latSide == 'N'){
        char lat_d[2];
        char lat_m[7];
        for (int z = 0; z < 2; z++) lat_d[z] = values[2][z];
        for (int z = 0; z < 6; z++) lat_m[z] = values[2][z + 2];

        int lat_deg_strtol = strtol(lat_d, NULL, 10);
        float lat_min_strtof = strtof(lat_m, NULL);
        double lat_deg = lat_deg_strtol + lat_min_strtof / 60;

        char lon_d[3];
        char lon_m[7];

        for (int z = 0; z < 3; z++) lon_d[z] = values[4][z];
        for (int z = 0; z < 6; z++) lon_m[z] = values[4][z + 3];

        int lon_deg_strtol = strtol(lon_d, NULL, 10);
        float lon_min_strtof = strtof(lon_m, NULL);
        double lon_deg = lon_deg_strtol + lon_min_strtof / 60;

        if(lat_deg!=0 && lon_deg!=0 && lat_deg<90 && lon_deg<180){
            gps_data->latitude = lat_deg;
            gps_data->latSide = latSide;
            gps_data->longitude = lon_deg;
            gps_data->lonSide = lonSide;
            float altitude = strtof(values[9], NULL);
            gps_data->altitude = altitude!=0 ? altitude : gps_data->altitude;
            gps_data->satelliteCount = strtol(values[7], NULL, 10);

            int fixQuality = strtol(values[6], NULL, 10);
            gps_data->fix = fixQuality > 0 ? 1 : 0;

            float hdop = strtof(values[8], NULL);
            gps_data->hdop = hdop!=0 ? hdop : gps_data->hdop;
        }
        else {
            for(int i=0; i<counter; i++) free(values[i]);
            return 0;
        }

    }

    for(int i=0; i<counter; i++) free(values[i]);
    return 1;
}


int nmea_GPGSA(GPS *gps_data, char*inputString){
    char *values[25];
    int counter = 0;
    memset(values, 0, sizeof(values));
    char *marker = strtok(inputString, ",");
    while (marker != NULL) {
        values[counter++] = malloc(strlen(marker) + 1); //free later!!!!!!
        strcpy(values[counter - 1], marker);
        marker = strtok(NULL, ",");
    }
    int fix = strtol(values[2], NULL, 10);
    gps_data->fix = fix > 1 ? 1 : 0;
    int satelliteCount = 0;
    for(int i=3; i<15; i++){
        if(values[i][0] != '\0'){
            satelliteCount++;
        }
    }
    gps_data->satelliteCount = satelliteCount;
    for(int i=0; i<counter; i++) free(values[i]);
    return 1;
}



int nmea_GNRMC(GPS *gps_data, char*inputString){
    char *values[25];
    int counter = 0;
    memset(values, 0, sizeof(values));
    char *marker = strtok(inputString, ",");
    while (marker != NULL) {
        values[counter++] = malloc(strlen(marker) + 1); //free later!!!!!!
        strcpy(values[counter - 1], marker);
        marker = strtok(NULL, ",");
    }
    float speed = strtof(values[7], NULL);
    gps_data->speed=speed/(1.944);


    for(int i=0; i<counter; i++) free(values[i]);
    return 1;
}



void nmea_parse(GPS *gps_data, uint8_t *buffer){
    memset(data, 0, sizeof(data));
    char * token = strtok(buffer, "$");
    int cnt = 0;
    while(token !=NULL){
        data[cnt++] = malloc(strlen(token)+1); //free later!!!!!
        strcpy(data[cnt-1], token);
        token = strtok(NULL, "$");
    }
    for(int i = 0; i<cnt; i++){
       if(strstr(data[i], "\r\n")!=NULL && gps_checksum(data[i])){
           if(strstr(data[i], "GNRMC")!=NULL){
               nmea_GNRMC(gps_data, data[i]);
           }
           else if(strstr(data[i], "GNGSA")!=NULL){
               nmea_GPGSA(gps_data, data[i]);
           }
           else if(strstr(data[i], "GNGGA")!=NULL){
               nmea_GPGGA(gps_data, data[i]);
           }
       }

    }
    for(int i = 0; i<cnt; i++) free(data[i]);


}

void nmea_speed(GPS *gps_data, uint8_t *buffer){
	 memset(data, 0, sizeof(data));
	    char * token = strtok(buffer, "$");
	    int cnt = 0;
	    while(token !=NULL){
	        data[cnt++] = malloc(strlen(token)+1); //free later!!!!!
	        strcpy(data[cnt-1], token);
	        token = strtok(NULL, "$");
	    }
	    for(int i = 0; i<cnt; i++){
	       if(strstr(data[i], "\r\n")!=NULL && gps_checksum(data[i])){
	           if(strstr(data[i], "GNRMC")!=NULL){
	               nmea_GNRMC(gps_data, data[i]);
	           }
	           else if(strstr(data[i], "GNGSA")!=NULL){
	                          nmea_GPGSA(gps_data, data[i]);
	                      }
	       }

	    }
	    for(int i = 0; i<cnt; i++) free(data[i]);

}



double distancecalc(double lat1, double lat2, double long1, double long2){
	double distance=0;
	distance=(double) 6371000*acosl(fmin(1,sinl(lat1*(M_PI/180))*sinl(lat2*(M_PI/180))+cosl(lat1*(M_PI/180))*cosl(lat2*(M_PI/180))*cosl((long2-long1)*(M_PI/180))));
	*distanceparcouru=*distanceparcouru + distance;

	return distance;
}













