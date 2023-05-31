#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include <linux/input.h>
#include <linux/joystick.h>

#define PORT 7777

int main(int argc, char **argv)
{

	//---INITIALIZATION---
	int server_fd, fd_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char send_buffer[1024];
	char recv_buffer[1024];

	// Creating socket file descriptor
	assert((server_fd = socket(AF_INET, SOCK_STREAM, 0)) != 0);

	// Forcefully attaching socket to the port 7777
	assert((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
					   sizeof(opt))) == 0);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 7777
	assert(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) >= 0);

	assert(listen(server_fd, 3) >= 0);

	// Establish connection
	assert((fd_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0);
	//---INITIALIZATION_END---

	//---CREATE_ADC_PROCESS---
	int fd_adc_stdin[2];
	int fd_adc_stdout[2];

	pipe(fd_adc_stdin);
	pipe(fd_adc_stdout);

	int pid_adc;
	char *argv_adc[] = {"/root/sensors", NULL};
	char *envp_adc[] = {NULL};

	pid_adc = fork();
	if (pid_adc == 0)
	{

		dup2(fd_adc_stdin[0], 0);
		dup2(fd_adc_stdout[1], 1);
		close(fd_adc_stdin[1]);
		close(fd_adc_stdout[0]);

		execve(argv_adc[0], argv_adc, envp_adc);
		exit(44);
	}
	//---CREATE_ADC_PROCESS_END---

	//---CREATE_WHEELS_PROCESS---
	int fd_wheels_stdin[2];

	pipe(fd_wheels_stdin);

	int pid_wheels;
	char *argv_wheels[] = {"/root/wheels_control", NULL};
	char *envp_wheels[] = {NULL};

	pid_wheels = fork();
	if (pid_wheels == 0)
	{

		dup2(fd_wheels_stdin[0], 0);
		close(fd_wheels_stdin[1]);

		execve(argv_wheels[0], argv_wheels, envp_wheels);
		exit(44);
	}
	//---CREATE_WHEELS_PROCESS_END---

	//---SERVER_LOOP_START---

	struct timeval delay, past_delay, time;
	memset(&delay, 0x0, sizeof(struct timeval));
	memset(&past_delay, 0x0, sizeof(struct timeval));
	memset(&time, 0x0, sizeof(struct timeval));

	fcntl(fd_adc_stdout[0], F_SETFL, O_NONBLOCK);
	fcntl(fd_socket, F_SETFL, O_NONBLOCK);
	char buf[4] = {0};

	int new_data = 0;
	int bytes_read = 0;
	const char prompt[] = {'>', 0};
	int offset = 0;
	struct js_event *js;
	js = recv_buffer;
	printf("Starting server loop...\n");
	// Read write loop
	write(fd_socket, prompt, 1);
	while (1)
	{

		//---READ_FROM_SENSORES---
		if (read(fd_adc_stdout[0], send_buffer + 1, 8) != -1)
		{
			send_buffer[0] = 0x02; // byte message
			send_buffer[9] = 0x00;
			offset = 10;
			new_data = 1;
		}
		else
		{
			new_data = 0;
		}
		//---READ_FROM_SENSROS_END---

		//---READ_FROM_SOCKET---
		if ((bytes_read = read(fd_socket, recv_buffer, 1024)) != -1)
		{


			// POWER
			if (js->type == 2 && js->number == 5)
			{
				buf[0] = 0x01;
				buf[1] = (uint8_t)(js->value);
				buf[2] = (uint8_t)(js->value >> 8);
				write(fd_wheels_stdin[1], buf, 3);
			}
			// X
			if (js->type == 2 && js->number == 4)
			{
				buf[0] = 0x02;
				buf[1] = (uint8_t)(js->value);
				buf[2] = (uint8_t)(js->value >> 8);
				write(fd_wheels_stdin[1], buf, 3);
			}
			// Y
			if (js->type == 2 && js->number == 3)
			{
				buf[0] = 0x03;
				buf[1] = (uint8_t)(js->value);
				buf[2] = (uint8_t)(js->value >> 8);
				write(fd_wheels_stdin[1], buf, 3);
			}
			// Exit button
			if (js->type == 1 && js->number == 6 && js->value == 1)
			{
				write(1, "Exiting...\n", 11);
				write(fd_socket, "\nExiting...\n", 12);

				break;
			}
		}
		//---READ_FROM_SOCKET_END---

		//---SEND_DATA---
		if (new_data == 1)
		{
			memcpy(send_buffer + offset, prompt, 2);
			gettimeofday(&delay, 0);
			if (past_delay.tv_sec != 0)
			{
				time.tv_sec = delay.tv_sec - past_delay.tv_sec;
				time.tv_usec = delay.tv_usec - past_delay.tv_usec;
			}
			else
			{
				time.tv_sec = 0;
				time.tv_usec = 0;
			}
			memcpy(&past_delay, &delay, sizeof(struct timeval));
			memcpy(send_buffer + 9, &time, sizeof(struct timeval));
			send_buffer[9 + sizeof(struct timeval)] = 0x00;

			write(fd_socket, send_buffer, 9 + sizeof(struct timeval) + 1);
			memset(send_buffer, 0x0, sizeof(send_buffer));
			memset(recv_buffer, 0x0, sizeof(recv_buffer));
			offset = 0;
		}
		//---SEND_DATA_END---

	}
	//---SERVER_LOOP_END---
	kill(pid_wheels, SIGKILL);
	write(fd_adc_stdin[1], "END", 3); // Terminating child process
	close(fd_socket);

	return 0;
}
