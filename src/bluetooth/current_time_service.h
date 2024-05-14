#ifdef __cplusplus
extern "C" {
#endif

struct current_time {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
};

struct current_time get_current_time(void);
void set_current_time(struct current_time time);

#ifdef __cplusplus
}
#endif
