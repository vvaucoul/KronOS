/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 15:40:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/20 17:10:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/tty.h>
#include <kernel.h>
#include <string.h>
#include <workflows/workflows.h>

/**
 * @brief Function to generate the header for a workflow.
 *
 * This function takes in a function name as a parameter and generates the header for a workflow.
 *
 * @param function_name The name of the function.
 */
void workflow_header(const char *function_name) {
	const uint32_t len = strlen(function_name);
	const uint32_t len_line = 80;
	const uint32_t len_line_start = (len_line - len) / 2 - 1;
	const uint32_t len_line_end = len_line - len - len_line_start - 2;

	printk("\n"_GREEN);
	for (uint32_t i = 0; i < len_line_start; ++i)
		putchar(WORKFLOW_CHAR);
	printk(_END " %s "_GREEN, function_name);
	for (uint32_t i = 0; i < len_line_end; ++i)
		putchar(WORKFLOW_CHAR);
	printk(_END "\n\n");

	tty_disable_cursor();
}

/**
 * @brief Function to add a footer to the workflow.
 *
 * This function adds a footer to the workflow. It is responsible for appending
 * the footer content to the existing workflow.
 */
void workflow_footer(void) {
	printk(_GREEN "\n");
	for (uint32_t i = 0; i < 80; ++i)
		putchar(WORKFLOW_CHAR);
	printk(_END "\n\n");

	tty_enable_cursor();
}

#define BAR_LENGTH 40

/**
 * @brief Function for handling the loading of a workflow.
 *
 * This function is responsible for handling the loading of a workflow and updating the progress.
 *
 * @param new_progress The new progress value for the workflow.
 */
void workflow_loading(uint32_t new_progress) {
	static uint32_t current_progress = 0;

	// Update the current progress
	if (new_progress > 100) {
		new_progress = 100;
	}
	current_progress = new_progress;

	// Calculate the number of characters to display in the progress bar
	uint32_t num_chars = (current_progress * BAR_LENGTH) / 100;

	// Display the progress bar
	for (uint32_t i = 0; i < BAR_LENGTH; ++i) {
		if (i < num_chars) {
			printk(_BG_LGREEN " ");
		} else {
			printk(_BG_BLACK " ");
		}
	}
	printk(_END " %3u%%%c\n", current_progress, '%'); // %3u for formatting and \r to return to the start of the line
	if (new_progress < 100) {
		printk(_CURSOR_MOVE_UP);
	} else {
		// Todo: printk clear line
		printk(_CURSOR_MOVE_UP);
		printk("                                                              \n");
		printk(_CURSOR_MOVE_UP);
	}
}

#undef BAR_LENGTH