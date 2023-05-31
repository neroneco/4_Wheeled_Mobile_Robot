#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <linux/input.h>
#include <linux/joystick.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"

#define PORT 7777

#define SETUP 1
#define RUNNING 2
#define STOPPED 3
#define ENDING 4

#define GREATER 1
#define LESS 2

#define YES 1
#define NO 0
#define SAVED 2
#define MANUAL 3

int main(int argc, char **argv)
{

	//---INITIALIZATION---
	int fd_socket = 0;
	struct sockaddr_in serv_addr;
	char recv_buffer[1024];
	char send_buffer[1024];
	assert((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	assert(inet_pton(AF_INET, "172.24.1.1", &serv_addr.sin_addr) > 0);

	assert(connect(fd_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0);
	//---INITIALIZATION_END---

	//---CREATE_CONTROLLER_PROCESS---
	int fd_controller_stdin[2];
	int fd_controller_stdout[2];

	pipe(fd_controller_stdin);
	pipe(fd_controller_stdout);

	int pid_controller;
	char *argv_controller[] = {"/home/martin/praca_inz/bin_x86_64/controller", NULL};
	char *envp_controller[] = {NULL};

	pid_controller = fork();
	if (pid_controller == 0)
	{

		dup2(fd_controller_stdin[0], 0);
		dup2(fd_controller_stdout[1], 1);
		close(fd_controller_stdin[1]);
		close(fd_controller_stdout[0]);

		execve(argv_controller[0], argv_controller, envp_controller);
		exit(33);
	}
	//---CREATE_CONTROLLER_PROCESS_END---

	//---CLIENT_LOOP_START---
	fcntl(fd_controller_stdout[0], F_SETFL, O_NONBLOCK);
	fcntl(fd_socket, F_SETFL, O_NONBLOCK);
	int read_bytes_server;
	printf("Starting Client loop...\n");

	struct object *acce;
	acce = malloc(sizeof(struct object));

	// Client loop
	char *mem_data;
	mem_data = malloc(8100000);
	int mem_offset = 0;
	struct js_event js_data, auto_js, past_data;
	int auto_mode = 0;
	int status = RUNNING;
	int distance = 0;
	float ac1_dist, ac2_dist;
	int save_data = NO;
	struct timeval begin, end, time;
	float time_vector = 0;
	int read_status = 0;
	memset(&js_data, 0x0, sizeof(struct js_event));
	memset(&auto_js, 0x0, sizeof(struct js_event));
	memset(&past_data, 0x0, sizeof(struct js_event));
	memset(recv_buffer, 0x0, 1024);
	memset(send_buffer, 0x0, 1024);
	while (1)
	{

		if ((read_bytes_server = read(fd_socket, recv_buffer, sizeof(recv_buffer))) != -1)
		{
			if (recv_buffer[0] == 0x02)
			{

				acce->raw[X].LSB = recv_buffer[3];
				acce->raw[X].MSB = recv_buffer[4];
				acce->raw[Y].LSB = recv_buffer[5];
				acce->raw[Y].MSB = recv_buffer[6];
				acce->raw[Z].LSB = recv_buffer[7];
				acce->raw[Z].MSB = recv_buffer[8];

				acce->values[X] = acce_convert(acce->raw[X]);
				acce->values[Y] = acce_convert(acce->raw[Y]);
				acce->values[Z] = acce_convert(acce->raw[Z]);

				/* It is possible to measure only         */
				/* Roll and Pitch with the accelerometer: */
				acce->degrees[X] = vector_to_degree(acce->values[X], acce->values[Y], acce->values[Z]);
				acce->degrees[Y] = vector_to_degree(acce->values[Y], acce->values[X], acce->values[Z]);

				ac1_dist = (((float)(uint8_t)recv_buffer[1]) / 255) * 3.3;
				ac2_dist = (((float)(uint8_t)recv_buffer[2]) / 255) * 3.3;

				memcpy(&time, recv_buffer + 9, sizeof(struct timeval));
				float elapsed = time.tv_sec + time.tv_usec * 1e-6;

				if (argc > 1)
				{
					if (argv[1][0] == '0')
					{
						//sensors data (RAW BYTES)
						printf("CH0_ADC=0x%hhx, "
							   "CH1_ADC=0x%hhx, "
							   "X_LSB=0x%hhx, "
							   "X_MSB=0x%hhx, "
							   "Y_LSB=0x%hhx, "
							   "Y_MSB=0x%hhx, "
							   "Z_LSB=0x%hhx, "
							   "Z_MSB=0x%hhx\n",
							   recv_buffer[1], recv_buffer[2], recv_buffer[3], recv_buffer[4], recv_buffer[5], recv_buffer[6], recv_buffer[7], recv_buffer[8]);
					}
					if (argv[1][0] == '1')
					{
						//sensors data only adc(VOLTS)
						printf("CH0_ADC=%.2f \n"
							   "CH1_ADC=%.2f \n",
							   ac1_dist, ac2_dist);
					}
					if (argv[1][0] == '2')
					{
						printf(
							"X=%.3f, "
							"Y=%.3f \n",
							acce->degrees[X], acce->degrees[Y]);
					}
					if (argv[1][0] == 'a')
					{
						if (argv[1][1] == 'l')
						{
							printf("%.2f\t"
								   "%.2f\t"
								   "%.3f\t"
								   "%.3f\n",
								   ac1_dist, ac2_dist, acce->degrees[X], acce->degrees[Y]);
						}
						else
						{
							printf("CH0_ADC=%.2f,"
								   "CH1_ADC=%.2f,"
								   "X=%.3f, "
								   "Y=%.3f, "
								   "time=%.6f\n",
								   ac1_dist, ac2_dist, acce->degrees[X], acce->degrees[Y], elapsed);
						}
					}
				}

				if (ac1_dist < 1.5)
				{
					distance = GREATER;
				}
				else
				{
					distance = LESS;
				}

				if (save_data == YES)
				{
					
					time_vector = time_vector+elapsed;
					mem_offset = sprintf(mem_data + mem_offset, "%.2f\t"
																"%.2f\t"
																"%.3f\t"
																"%.3f\t"
																"%.6f\n",
										ac1_dist, ac2_dist, acce->degrees[X], acce->degrees[Y], time_vector) +
								mem_offset;
					
					if (mem_offset > 8000000)
					{
						printf("\nSaving data stopped.(AUTO)\n"
							   "Data in memory buffer, waiting to finish program\n");
						save_data = SAVED;
					}
				}

				if (save_data == MANUAL)
				{
					
					putchar('.');
					time_vector = time_vector+elapsed;
					mem_offset = sprintf(mem_data + mem_offset, "%.2f\t"
																"%.2f\t"
																"%.3f\t"
																"%.3f\t"
																"%.6f\n",
										ac1_dist, ac2_dist, acce->degrees[X], acce->degrees[Y], time_vector) +
								mem_offset;
					
					if (time_vector > 10.0)
					{
						printf("\nSaving data stopped.(MANUAL)\n"
							   "Data in memory buffer, waiting to finish program\n");
						save_data = SAVED;
					}
				}
			}
			else
			{
				printf("%s\n", recv_buffer);
				if (strncmp(recv_buffer + 1, "Exit", 4) == 0)
				{
					break;
				}
			}

		}

		if ((read_status = read(fd_controller_stdout[0], send_buffer, sizeof(sizeof(struct js_event)))) != -1 || auto_mode == 1)
		{
			memcpy(&js_data, send_buffer, sizeof(struct js_event));

			if (js_data.type == 1)
			{
				if (js_data.value == 0 && past_data.value == 1)
				{
					if (js_data.number == 0)
					{ // A button
						// START Algorithm
						if (auto_mode == 0)
						{
							auto_mode = 1;
							status = SETUP;
							save_data = YES;
							printf("auto mode\n");
							printf("Algorithm started\n");
							printf("Saving data started...(AUTO)\n");
							gettimeofday(&begin, 0);
						}
						else if (auto_mode == 1)
						{
							auto_mode = 0;
							status = STOPPED;
							save_data = NO;
							printf("manual mode\n");
						}
					}
					else if (js_data.number == 1)
					{ // B button
						// STOP platoform or Algorithm in case
						if (auto_mode == 0)
						{
							// close file
						}
						else if (auto_mode == 1)
						{
							status = STOPPED;
							printf("Algorithm stopped\n");
						}
					}
					else if (js_data.number == 3)
					{ // Y button
						if (auto_mode == 0)
						{
							// open file and start saving data
							save_data = MANUAL;
							printf("Saving data started...(MANUAL)\n");
						}
						else if (auto_mode == 1)
						{
							status = SETUP;
							printf("Algorithm resumed\n");
						}
					}
					else
					{
					}
				}
				memcpy(&past_data, &js_data, sizeof(struct js_event));
			}

			if (auto_mode == 1 || status == STOPPED)
			{
				if (distance == LESS)
				{
					// STOP Algorithm
					// Turn motors OFF
					status = ENDING;
					auto_mode = 0;
					save_data = SAVED;

					auto_js.type = 2;
					auto_js.number = 5;
					auto_js.value = -32767;
					gettimeofday(&end, 0);
				}
				else if (distance == GREATER)
				{
					// Turn motors ON
					if (status == SETUP)
					{;
						auto_js.type = 2;
						auto_js.number = 5;
						auto_js.value = 500;
					}
					else if (status == STOPPED)
					{
						auto_js.type = 2;
						auto_js.number = 5;
						auto_js.value = -32767;
					}
				}
				if (status == ENDING || status == STOPPED || status == SETUP)
				{
					write(fd_socket, &auto_js, sizeof(struct js_event));
					if (status == SETUP || status == STOPPED)
					{
						status = RUNNING;
					}
				}
				if (status == ENDING)
				{
					printf("\nSaving data stopped.(AUTO)\n"
						   "Data in memory buffer, waiting to finish program\n");
				}
			}
			else
			{
				write(fd_socket, &js_data, sizeof(struct js_event));
			}
		}

		memset(recv_buffer, 0x0, 1024);
		memset(send_buffer, 0x0, 1024);
	}
	//---CLIENT_LOOP_END---
	int data_file_fd;
	if (save_data == SAVED)
	{
		printf("Savin data\n");
		if ((data_file_fd = open("data.txt", O_CREAT | O_WRONLY)) != -1)
		{
			write(data_file_fd, mem_data, mem_offset);
		}
		else
		{
			printf("Couldn't save data file\n");
			perror("open");
		}
	}

	free(mem_data);
	close(data_file_fd);
	write(fd_controller_stdin[1], "END", 3); // Terminating child process
	close(fd_socket);

	return 0;
}
