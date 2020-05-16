#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){

	char buffer[128];
	int len;
	
        // set non-blocking
        int flags = fcntl(0, F_GETFL, 0);
        fcntl(0, F_SETFL, flags | O_NONBLOCK);

	for (;;){

		len = read( 0, buffer, sizeof(buffer) );
		if (len == 0){
			break;
		}
		if (len > 0){
			if (len == 3){
				buffer[0] = (buffer[0] & 0xF0) | ((buffer[0] + 1) & 0x0F);
			}
			write( 1, buffer, len );
		}

	}
	
	return 0;
}
