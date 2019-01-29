#define FIO0DIR (*(volatile unsigned int *)0x2009c000)
#define FIO0PIN (*(volatile unsigned int *)0x2009c014)

int turn = 0;

void lights(int row, int num)
{
	switch(row)
	{
	//Red LEDs port p01,p00,p06,p07,p08,p09,p018
	case 0:
		switch(num)
		{
		case 1:

			break;
		case 2:

			break;
		case 3:

			break;
		case 4:

			break;
		case 5:

			break;
		case 6:

			break;
		case 7:

			break;
		default:

			break;
		}
		break;
		//Blue LEDs port p017,p015,p016,p023,p024
	case 1:
		switch(num)
		{
		case 1:

		    break;
		case 2:

		    break;
		case 3:

		    break;
		case 4:

		    break;
		case 5:

		    break;
		default:

		    break;
		}
		break;
		//Blue LEDs port p025,p026,p02
	case 2:
		switch(num)
		{
		case 1:

		    break;
		case 2:

		    break;
		case 3:

		    break;
		default:

		    break;
		}
		break;

	default:
		break;

	}
}

int main(void) {

	int red = 0;
	int green = 0;
	int blue = 0;

    while(1){



    }

}
