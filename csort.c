#include "sort.h"
#include "stdio.h"


int strcmp2(char *str1, char *str2);
void sort(char**array, unsigned int left, unsigned int right);

void sortC(char** array, unsigned int count) {
	
	sort(array, 0, count-1);
	
	return;
}

void sort(char** array, unsigned int left, unsigned int right){

	int l, pivot, r;
	l = left;
	r = right;
	pivot = (l+r)/2;
	char *pivotstr = array[pivot];
	
	while( l <= r){
		
		while(strcmp2(array[l],pivotstr) < 0){
			l++;
		}
		while(strcmp2(array[r],pivotstr) > 0){
			r--;
		}
		
		if(l <= r){
			char *temp = array[l];
			array[l] = array[r];
			array[r] = temp;
			l++;
			r--;
		}
	}

	if(left<r) sort(array,left,r);
	if(l<right) sort(array,l,right);
}

int strcmp2(char *str1, char *str2){
	
	int i = 0;
	
	while(1) {
		int result = str1[i] - str2[i];
		if(result != 0){
			return result;
		} else if (str1[i] == '\0'){
            return 0;
        } else {
			i++;
		}
	}
}
