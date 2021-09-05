import re

MAX_MATCHES = 10

f = open('benchout.txt')

pat1 = re.compile('Time')
pat2 = re.compile('Data sent')
pat3 = re.compile('Global data sent')
dnum = re.compile(r'\d+\.\d+')
matches = 0


def put_stats_in_lists(num_lines: int) -> (list, float, float):
	times = []
	matches = 0
	for line in f:
		if(pat1.match(line)):
			times.append(float(dnum.search(line).group()))
			matches += 1		
		if matches == num_lines:
			it = iter(f)
			data = float(dnum.search(next(it)).group())
			global_data = float(dnum.search(next(it)).group())
			return times, data, global_data

time_turbo, data_turbo, global_data_turbo = put_stats_in_lists(MAX_MATCHES)
time_spdz, data_spdz, global_data_spdz = put_stats_in_lists(MAX_MATCHES)

avg_time_turbo = sum(time_turbo)/len(time_turbo)
avg_time_spdz = sum(time_spdz)/len(time_spdz)

print('SPDZ:')
print('Average time: %f'% (avg_time_spdz))
print('Data sent for player1: %f'% (data_spdz))
print('Data sent for player2: %f'% (global_data_spdz - data_spdz))
print('Total data sent: %f'% (global_data_spdz))
print('\n')
print('Turbospeedz:')
print('Average time: %f'% (avg_time_turbo))
print('Data sent for player1: %f'% (data_turbo))
print('Data sent for player2: %f'% (global_data_turbo - data_turbo))
print('Total data sent: %f'% (global_data_turbo))