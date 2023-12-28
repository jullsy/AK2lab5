// SPDX-License-Identifier: GPL-2.0+
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

// Визначення структури для елемента списку
struct list_item {
	struct list_head next;  // Вказівник на наступний елемент списку
	ktime_t start_time;      // Час початку
	ktime_t finish_time;     // Час завершення
};

MODULE_AUTHOR("Yuliia Tyshkevych");        // Ім'я автора модуля
MODULE_DESCRIPTION("AK-2 Labwork 5");           // Опис модуля
MODULE_LICENSE("Dual BSD/GPL");       // Ліцензія
MODULE_VERSION("1.0");                 // Версія модуля

static LIST_HEAD(head);  // Голова списку

static uint p = 1;  // Параметр модуля (лічильник)
module_param(p, uint, 0444);
MODULE_PARM_DESC(p, "Counter (p)");  // Опис параметра

// Ініціалізація модуля
static int __init hello_init(void)
{
	uint i = 0;
	struct list_item *tail;

	pr_info("Count: %d\n", p);  // Виведення значення параметра

	if (!p)
		pr_warn("The parameter is 0");

	if (p >= 5 && p <= 10)
		pr_warn("The parameter is %d, between 5 and 10, enter number less than 5", p);

	BUG_ON(p > 10);  // Викидання помилки, якщо значення параметра більше 10

	for (i = 0; i < p; i++) {
		tail = kmalloc(sizeof(struct list_item), GFP_KERNEL);  // Виділення пам'яті для елемента списку

		if (i == 5)
			tail = 0;

		if (ZERO_OR_NULL_PTR(tail))
			goto exception;

		tail->start_time = ktime_get();  // Запис часу початку
		pr_info("Hello, World!\n");
		tail->finish_time = ktime_get();  // Запис часу завершення

		list_add_tail(&(tail->next), &head);  // Додавання елемента до кінця списку
	}

	return 0;

exception:
	{
		struct list_item *md, *tmp;

		pr_err("The end of memory...\n");

		// Обходження списку та видалення елементів
		list_for_each_entry_safe(md, tmp, &head, next) {
			list_del(&md->next);
			kfree(md);
		}

		BUG();  // Викидання помилки
		return -ENOMEM;  // Повернення помилки
	}
}

// Завершення роботи модуля
static void __exit hello_exit(void)
{
	struct list_item *md, *tmp;

	// Обходження списку та виведення часу виконання для кожного елемента
	list_for_each_entry_safe(md, tmp, &head, next) {
		pr_info("Time: %lld",
		md->finish_time - md->start_time);

		list_del(&md->next);  // Видалення елемента зі списку
		kfree(md);  // Звільнення пам'яті для елемента
	}

	BUG_ON(!list_empty(&head));  // Викидання помилки, якщо список не пустий
}

module_init(hello_init);
module_exit(hello_exit);
