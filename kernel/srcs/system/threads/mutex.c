

/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 11:35:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 11:51:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/process.h>
#include <system/mutex.h>

static void __add_task_to_wait_queue(task_t **wait_queue, task_t *task) {
    task_t *tmp = *wait_queue;
    if (!tmp) {
        *wait_queue = task;
        return;
    }
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = task;
}

static task_t *__remove_task_from_wait_queue(task_t **wait_queue) {
    task_t *tmp = *wait_queue;
    if (!tmp) {
        return NULL;
    }
    *wait_queue = tmp->next;
    tmp->next = NULL;
    return tmp;
}

void init_mutex(mutex_t *mutex) {
    mutex->state = MUTEX_UNLOCKED;
    mutex->owner = 0;
    mutex->wait_queue = NULL;
}

void acquire_mutex(mutex_t *mutex) {
    ASM_CLI();
    if (mutex->state) {
        if (get_current_task() && (mutex->owner != get_current_task()->owner)) {
            lock_task(get_current_task());
            __add_task_to_wait_queue(&mutex->wait_queue, get_current_task());
        }
    } else {
        mutex->state = MUTEX_LOCKED;
        mutex->owner = get_current_task()->owner;
    }
    ASM_STI();
}

void release_mutex(mutex_t *mutex) {
    ASM_CLI();
    if (mutex->state && mutex->owner == get_current_task()->owner) {
        mutex->state = MUTEX_UNLOCKED;
        mutex->owner = 0;
        if (mutex->wait_queue) {
            task_t *next_task = __remove_task_from_wait_queue(&mutex->wait_queue);
            mutex->state = MUTEX_LOCKED;
            mutex->owner = next_task->owner;
            unlock_task(next_task);
        }
    }
    ASM_STI();
}