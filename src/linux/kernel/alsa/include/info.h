#ifndef __INFO_H
#define __INFO_H

/*
 *  Header file for info interface
 *  Copyright (c) by Jaroslav Kysela <perex@suse.cz>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* buffer for information */

struct snd_info_buffer {
	char *buffer;		/* pointer to begin of buffer */
	char *curr;		/* current position in buffer */
	unsigned long size;	/* current size */
	unsigned long len;	/* total length of buffer */
	int stop;		/* stop flag */
	int error;		/* error code */
};

typedef struct snd_info_buffer snd_info_buffer_t;

#define SND_INFO_ENTRY_TEXT	0
#define SND_INFO_ENTRY_DATA	1
#define SND_INFO_ENTRY_SDEVICE  1	/* subtype */
#define SND_INFO_ENTRY_DEVICE	2
#define SND_INFO_ENTRY_SEQUENCER 10	/* subtype */

struct snd_info_entry;

struct snd_info_entry_text {
	unsigned long read_size;
	unsigned long write_size;
	void (*read) (snd_info_buffer_t * buffer, void *private_data);
	void (*write) (snd_info_buffer_t * buffer, void *private_data);
};

struct snd_info_entry_data {
	int (*open) (void *private_data, snd_info_entry_t * entry,
		     unsigned short mode, void **file_private_data);
	int (*release) (void *private_data, snd_info_entry_t * entry,
			unsigned short mode, void *file_private_data);
	long (*read) (void *private_data, void *file_private_data,
		      struct file * file, char *buf, long count);
	long (*write) (void *private_data, void *file_private_data,
		       struct file * file, const char *buf, long count);
	long long (*lseek) (void *private_data, void *file_private_data,
			    struct file * file, long long offset, int orig);
	unsigned int (*poll) (void *private_data, void *file_private_data,
			      struct file * file, poll_table * wait);
	int (*ioctl) (void *private_data, void *file_private_data,
		      struct file * file, unsigned int cmd, unsigned long arg);
	int (*mmap) (void *private_data, void *file_private_data,
		     struct inode * inode, struct file * file,
		     struct vm_area_struct * vma);
};

struct snd_info_entry_device {
	unsigned short major;
	unsigned short minor;
};

struct snd_info_entry {
	const char *name;
	mode_t mode;
	long size;
	unsigned short type;
	unsigned short subtype;
	union {
		struct snd_info_entry_text text;
		struct snd_info_entry_data data;
		struct snd_info_entry_device device;
	} t;
	void *private_data;
	void (*private_free)(void *private_data);
	void (*use_inc) (snd_card_t * card);
	void (*use_dec) (snd_card_t * card);
	snd_card_t *card;
	struct proc_dir_entry *p;
	struct snd_info_entry *next;
	struct semaphore access;
};

extern int snd_info_check_reserved_words(const char *str);

#ifdef CONFIG_SND_OSSEMUL
extern int snd_info_minor_register(void);
extern int snd_info_minor_unregister(void);
#endif


#ifdef CONFIG_PROC_FS

extern int snd_iprintf(snd_info_buffer_t * buffer, char *fmt,...);
extern int snd_info_init(void);
extern int snd_info_done(void);

extern int snd_info_get_line(snd_info_buffer_t * buffer, char *line, int len);
extern char *snd_info_get_str(char *dest, char *src, int len);
extern snd_info_entry_t *snd_info_create_entry(snd_card_t * card, const char *name);
extern void snd_info_free_entry(snd_info_entry_t * entry);
extern snd_info_entry_t *snd_info_create_device(const char *name,
					        unsigned int number,
					        unsigned int mode);
extern void snd_info_free_device(snd_info_entry_t * entry);
extern int snd_info_store_text(snd_info_entry_t * entry);
extern int snd_info_restore_text(snd_info_entry_t * entry);

extern int snd_info_card_register(snd_card_t * card);
extern int snd_info_card_unregister(snd_card_t * card);
extern int snd_info_register(snd_info_entry_t * entry);
extern int snd_info_unregister(snd_info_entry_t * entry);

#else

static inline int snd_iprintf(snd_info_buffer_t * buffer, char *fmt,...) { return 0; }
static inline int snd_info_init(void) { return 0; }
static inline int snd_info_done(void) { return 0; }

static inline int snd_info_get_line(snd_info_buffer_t * buffer, char *line, int len) { return 0; }
static inline char *snd_info_get_str(char *dest, char *src, int len) { return NULL; }
static inline snd_info_entry_t *snd_info_create_entry(snd_card_t * card, const char *name) { return NULL; }
static inline void snd_info_free_entry(snd_info_entry_t * entry) { ; }
static inline snd_info_entry_t *snd_info_create_device(const char *name,
						       unsigned int number,
						       unsigned int mode) { return NULL; }
static inline void snd_info_free_device(snd_info_entry_t * entry) { ; }
static inline int snd_info_store_text(snd_info_entry_t * entry) { return 0; }
static inline int snd_info_restore_text(snd_info_entry_t * entry) { return 0; }

static inline int snd_info_card_register(snd_card_t * card) { return 0; }
static inline int snd_info_card_unregister(snd_card_t * card) { return 0; }
static inline int snd_info_register(snd_info_entry_t * entry) { return 0; }
static inline int snd_info_unregister(snd_info_entry_t * entry) { return 0; }


#endif

/*
 * OSS info part
 */

#ifdef CONFIG_SND_OSSEMUL

#define SND_OSS_INFO_DEV_AUDIO	0
#define SND_OSS_INFO_DEV_SYNTH	1
#define SND_OSS_INFO_DEV_MIDI	2
#define SND_OSS_INFO_DEV_TIMERS	4
#define SND_OSS_INFO_DEV_MIXERS	5

#define SND_OSS_INFO_DEV_COUNT	6

extern int snd_oss_info_register(int dev, int num, char *string);
#define snd_oss_info_unregister(dev, num) snd_oss_info_register(dev, num, NULL)

#endif				/* CONFIG_SND_OSSEMUL */

#endif				/* __INFO_H */