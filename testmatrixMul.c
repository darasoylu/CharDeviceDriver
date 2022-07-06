#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

static char message[256]; // The message buffer from the LKM

int main() {
  int ret, fd, i, M, N;

  printf("İlk matrisin satır sayısını girin:");
  scanf("%d", & M);
  printf("İlk matrisin sütun sayısını girin:");
  scanf("%d", & N);

  int matrix1[M * N], matrix2[N];

  printf("İlk matrisin elemanlarını girin:\n");
  for (i = 0; i < M * N; i++) {
    scanf("%d", & matrix1[i]);
  }

  printf("İkinci matrisi girin:\n");
  for (i = 0; i < N; i++) {
    scanf("%d", & matrix2[i]);
  }

  char * temp = (char * ) malloc(sizeof(char) * (sizeof(matrix1)));

  char * arrchar = (char * ) malloc(sizeof(char) * (sizeof(matrix1)));
  for (i = 0; i < M * N; i++) {
    sprintf(temp, "%d ", matrix1[i]); //Matrix1'i parse ediyoruz ve arrchar'da tutuyoruz.
    strcat(arrchar, temp);
  }

  char * arrchar2 = (char * ) malloc(sizeof(char) * (sizeof(matrix2)));
  for (i = 0; i < N; i++) {
    sprintf(temp, "%d ", matrix2[i]); //Matrix2'i parse ediyoruz ve arrchar2'de tutuyoruz.
    strcat(arrchar2, temp);
  }

  sprintf(message, "%d %d*%s*%s*", M, N, arrchar, arrchar2); //Bu stringi "M N*matrix1*matrix2*" formatina ceviriyoruz.(M->satir, N->sutun, matrix1->arrchar, matrix2->arrchar2)

  printf("Starting device\n");
  fd = open("/dev/matrixMulDevice", O_RDWR); // Open the device with read/write access
  if (fd < 0) {
    perror("Failed to open the device");
    return errno;
  }

  ret = write(fd, message, strlen(message)); // Send the string to the LKM
  if (ret < 0) {
    perror("Failed to write the message to the device.");
    return errno;
  }

  printf("End of the program\n");

  return 0;
}
