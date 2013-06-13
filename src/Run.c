/*
 * Run.c
 *
 *  Created on: 06.06.2013
 *      Author: Alexander
 */


int main(int argc, char *argv[]){
	int ret = 0;
	ret = simulation();
	reconstruction();
	return ret;
}

