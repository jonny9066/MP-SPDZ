import re

MAX_MATCHES = 10

f = open('benchout.txt')

pat1 = re.compile('Time')
dnum = re.compile(r'\d+\.\d+')
matches = 0


def parse_results(num_lines: int) -> (list, float, float):
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

time_turbo, data_turbo, global_data_turbo = parse_results(MAX_MATCHES)
time_spdz, data_spdz, global_data_spdz = parse_results(MAX_MATCHES)

avg_time_turbo = sum(time_turbo)/len(time_turbo)
avg_time_spdz = sum(time_spdz)/len(time_spdz)

print('SPDZ:')
print('Average time: %f seconds'% (avg_time_spdz))
print('Data sent for player1 MB: %f'% (data_spdz))
print('Data sent for player2 MB: %f'% (global_data_spdz - data_spdz))
print('Total data sent MB: %f'% (global_data_spdz))
print('')
print('Turbospeedz:')
print('Average time: %f seconds'% (avg_time_turbo))
print('Data sent for player1: %f MB'% (data_turbo))
print('Data sent for player2: %f MB'% (global_data_turbo - data_turbo))
print('Total data sent: %f MB'% (global_data_turbo))