#include "thread.h"
#include "thread-sync.h"

int n, count = 0;
mutex_t lk = MUTEX_INIT();
cond_t cv = COND_INIT();

// Bug?
cond_t cv2 = COND_INIT();

void Tproduce() 
{
	while (1) 
	{
		mutex_lock(&lk);
		
		while (count == n) 
		{
			cond_wait(&cv2, &lk);
		}

		assert(count != n);
		printf("("); 
		count++;
		cond_signal(&cv);
		mutex_unlock(&lk);
	}
}

void Tconsume() 
{
	while (1) 
	{
		mutex_lock(&lk);

		while (count == 0) 
		{
			cond_wait(&cv, &lk);
		}

		assert(count != 0);
		printf(")"); 
		count--;
		cond_signal(&cv2);
		mutex_unlock(&lk);
	}
}

int main(int argc, char *argv[]) 
{
	assert(argc == 2);

	n = atoi(argv[1]);
	setbuf(stdout, NULL);

	for (int i = 0; i < 2; i++) 
	{
		create(Tproduce);
		create(Tconsume);
	}
}
