#include "pch.h"
#include <iostream>
#include <time.h>
#include <omp.h>
#include <Windows.h>
using namespace std;


int ** get_A_block(int **A, int position1, int position2, int k)
{
	int **D = new int*[k];
	int x = position1 * k;
	int y = position2 * k;
	for (int i = 0; i < k; i++)
	{
		D[i] = new int[k];
		for (int j = 0; j < k; j++)
		{
			D[i][j] = A[i + x][j + y];
		}
	}
	return D;
}


int ** get_B_block(int **B, int position1, int position2, int k)
{
	int **D = new int*[k];
	int x = position1 * k;
	int y = position2 * k;
	for (int i = 0; i < k; i++)
	{
		D[i] = new int[k];
		for (int j = 0; j < k; j++)
		{
			D[i][j] = B[i + y][j + x];
		}
	}
	return D;
}


int ** create_rect_matrix(int **A, int n, int *sub_A, int count)
{
	int chet = n;
	int k = 0;
	int sum = 0;
	
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			A[i][j] = 0;
		}
		for (int j = i; j < n; j++)
		{
			A[i][j] = sub_A[k];
			k++;
		}
	}
	return A;
}



int main()
{
	const int n = 256;
	int count = 0;
	for (int i = 1; i < n+1;  i++) {
		count += i;
	}
	int *sub_A = new int[count];
	int *sub_B = new int[count];
	for (int i = 0; i < count; i++) {
		sub_A[i] = rand() % 9;
		sub_B[i] = rand() % 9;
	}
	
	int **A = new int *[n];
	int **B = new int *[n];
	int **D = new int *[n];
	
	for (int i = 0; i < n; i++)
	{
		A[i] = new int[n];
		B[i] = new int[n];
		D[i] = new int[n];
		for (int j = 0; j < n; j++)
		{
			A[i][j] = rand() % 9;
			B[i][j] = rand() % 9;
			A[i][j] = 0;
			B[i][j] = 0;
			D[i][j] = 0;
		}
	}

	
	double start_time, end_time;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			cout << "block " << k << "\n";
			cout << "with parallel: ";
			start_time = omp_get_wtime();
			for (int block_row = 0; block_row < int(n/k); block_row++)
			{
			
					for (int block_column = 0; block_column < int(n/k); block_column++)
					{
						
#pragma omp parallel num_threads(4) //private(block_column, i, j)
							{
#pragma omp for  schedule(static)
								for (int block_column2 = 0; block_column2 < n / k; block_column2++)
								{
									for (int i = 0; i < k; i++)
									{
										for (int j = 0; j < k; j++)
										{
											for (int f = 0; f < k; f++) {
												D[block_row*k + i][block_column2*k + j] += A[block_row*k + i][block_column*k + f] * B[block_column*k + f][block_column2*k + j];
											}
										}
									}
								}
							}

						
					}
				
			}
			end_time = omp_get_wtime();
			cout << end_time - start_time << "\n";
			

			cout << "with parallel in single block: ";
			start_time = omp_get_wtime();
			for (int block_row = 0; block_row < int(n / k); block_row++)
			{

					for (int block_column = 0; block_column < int(n / k); block_column++)
					{
						#pragma omp parallel num_threads(4) //private(block_column, i, j)
						{
							#pragma omp for  schedule(static)
							for (int block_column2 = 0; block_column2 < n / k; block_column2++)
							{
								int ** sub_A = get_A_block(A, block_row, block_column, k);
								int ** sub_B = get_B_block(A, block_column, block_column2, k);
								for (int i = 0; i < k; i++) {
									for (int j = 0; j < k; j++)
									{
										for (int f = 0; f < k; f++) {
											D[block_row*k + i][block_column2*k + j] += sub_A[i][j] * sub_B[j][f];
										}
									}
								}
								for (int i = 0; i < k; ++i) {
									delete[] sub_A[i];
									delete[] sub_B[i];
								}
								delete[] sub_A;
								delete[] sub_B;
							}
						}
					}
				
			}
			end_time = omp_get_wtime();
			cout << end_time - start_time << "\n";
			
			cout << "no parralel ";
			start_time = omp_get_wtime();
			for (int block_row = 0; block_row < n / k; block_row++)
			{
				for (int block_column = 0; block_column < n / k; block_column++)
				{
					for (int block_column2 = 0; block_column2 < n / k; block_column2++)
					{
						for (int i = 0; i < k; i++)
						{
							for (int j = 0; j < k; j++)
							{
								for (int f = 0; f < k; f++) {
									D[block_row*k + i][block_column2*k + j] += A[block_row*k + i][block_column*k + f] * B[block_column*k + f][block_column2*k + j];
								}
							}
						}
					}

				}
			}
			end_time = omp_get_wtime();
			cout << "   " <<end_time - start_time << "\n";



			start_time = omp_get_wtime();
			cout << "no parallel in single block: ";
			for (int block_row = 0; block_row < n / k; block_row++)
			{
				for (int block_column = 0; block_column < n / k; block_column++)
				{
					
						for (int block_column2 = 0; block_column2 < n / k; block_column2++)
						{
							int ** sub_A = get_A_block(A, block_row, block_column, k);
							int ** sub_B = get_B_block(A, block_column, block_column2, k);
							for (int i = 0; i < k; i++) {
								for (int j = 0; j < k; j++)
								{
									for (int f = 0; f < k; f++) {
										D[block_row*k + i][block_column2*k + j] += sub_A[i][j] * sub_B[j][f];
									}
								}
							}
							for (int i = 0; i < k; ++i) {
								delete[] sub_A[i];
								delete[] sub_B[i];
							}
							delete[] sub_A;
							delete[] sub_B;
						}
					
				}

			}
			end_time = omp_get_wtime();
			cout << "   " << end_time - start_time << "\n";
			cout << "***************************" << "\n";
		}
	}

	for (int i = 0; i < n; ++i) {
		delete[] A[i];
		delete[] B[i];
		delete[] D[i];
	}
	delete[] A;
	delete[] B;
	delete[] D;


	return 0;

}