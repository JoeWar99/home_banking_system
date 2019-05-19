#include "active_offices.h"

#include <stdio.h>
#include "sync.h"

static int active_offices = 0;

int get_active_offices(){
	return active_offices;
}

int increase_active_offices(){
	int ret;
	/* Wait mutex */
	if((ret = lock_active_office_mutex()) != 0){
		fprintf(stderr, "lock_active_office_mutex: error %d\n", ret);
		return ret;
	}

	active_offices++;

	/* Signal mutex */
	if((ret = unlock_active_office_mutex()) != 0)
		fprintf(stderr, "unlock_active_office_mutex: error %d\n", ret);
	
	return ret;
}

int decrease_active_offices(){
	int ret;
	/* Wait mutex */
	if((ret = lock_active_office_mutex()) != 0){
		fprintf(stderr, "lock_active_office_mutex: error %d\n", ret);
		return ret;
	}

	active_offices--;

	/* Signal mutex */
	if((ret = unlock_active_office_mutex()) != 0)
		fprintf(stderr, "unlock_active_office_mutex: error %d\n", ret);
	
	return ret;
}