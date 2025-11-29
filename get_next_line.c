/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Visual <github.com/visual-gh>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 18:17:21 by Visual            #+#    #+#             */
/*   Updated: 2025/11/29 17:05:04 by Visual           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

// STEP 1: Read from file descriptor until we find '\n' or reach EOF
// Builds up the 'stash' by reading BUFFER_SIZE chunks at a time
// The stash accumulates all data read so far (including leftovers from previous calls)

static char	*read_to_stash(int fd, char *stash)
{
	char	*buffer;
	char	*temp;
	ssize_t	n;

	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	n = 1;
	while (!ft_strchr(stash, '\n') && n > 0)
	{
		n = read(fd, buffer, BUFFER_SIZE);
		if (n < 0)
			return (free(buffer), free(stash), NULL);
		buffer[n] = '\0';
		if (!stash)
			stash = ft_strdup("");
		temp = ft_strjoin(stash, buffer);
		free(stash);
		stash = temp;
		if (!stash)
			return (free(buffer), NULL);
	}
	free(buffer);
	return (stash);
}

// STEP 2: Extract one complete line from the stash
// A line ends with '\n' OR at the end of the stash (last line without \n)
// Returns a newly allocated string containing the line (including \n if present)

static char	*extract_line(char *stash)
{
	char	*line;
	size_t	i;

	if (!stash || !stash[0])
		return (NULL);
	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	line = malloc(i + 2);
	if (!line)
		return (NULL);
	i = 0;
	while (stash[i] && stash[i] != '\n')
	{
		line[i] = stash[i];
		i++;
	}
	if (stash[i] == '\n')
		line[i++] = '\n';
	line[i] = '\0';
	return (line);
}

// STEP 3: Clean the stash by removing the line we just extracted
// Keeps everything AFTER the '\n' for the next call to get_next_line
// If there's nothing left after '\n', free the stash and return NULL

static char	*clean_stash(char *stash)
{
	char	*new;
	size_t	i;
	size_t	j;

	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (!stash[i])
		return (free(stash), NULL);
	new = malloc(ft_strlen(stash) - i + 1);
	if (!new)
		return (free(stash), NULL);
	i++;
	j = 0;
	while (stash[i])
		new[j++] = stash[i++];
	new[j] = '\0';
	free(stash);
	return (new);
}

// MAIN FUNCTION: Called repeatedly to read file line by line
// Uses a static variable 'stash' to remember leftover data between calls
// Returns one line per call, NULL when file ends or on error

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	stash = read_to_stash(fd, stash);
	if (!stash)
		return (NULL);
	line = extract_line(stash);
	stash = clean_stash(stash);
	return (line);
}
