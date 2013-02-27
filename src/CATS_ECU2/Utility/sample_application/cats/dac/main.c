/*******************************************************************************

	main.c

	sample application / basic usage of the APIs


*******************************************************************************/

#include <stdio.h>

#include "/usr/src/CATS_ECU2/api_library/dacapi/dac_api.h"


/*******************************************************************
機能 			: 	main
引数			:	無し
戻り値			:	無し
機能説明		:	
*******************************************************************/
int main(void)
{
	unsigned int ret,tmp,atmp[16],atmp2[16],i;
	unsigned char c;

	// DAC test
	ret = FncInitDAC();
	printf("FncInitDAC ret = %d\n",ret);
	for(i=0;i<8;i++){
		atmp[i] = 0x800;
	}

	tmp = 0;

	while(tmp<0x1000){
		usleep(1000);
		atmp[0] = tmp;
		atmp[4] = tmp;
		ret = FncDACSet ( atmp );
		printf("FncDACSet 0Ch,4Ch = %x ret = %d\n",atmp[4] ,ret);
		if (ret !=0){
			ret = FncDACStop();
			printf("FncDACStop ret = %d\n",ret);
			return 0;
		}

	    tmp++;
	}

	sleep(5);

	ret = FncDACStop();
	printf("FncDACStop ret = %d\n",ret);


	return 0;
}
