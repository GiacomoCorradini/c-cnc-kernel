% Analysze execution time of timed* examples
clc
clear
close all

%% Read file
timed0_csv = readtable('timed1.csv');
timed2_csv = readtable('timed2.csv');

%% Plot
figure
subplot(2, 1, 1)
plot(timed0_csv.n, timed0_csv.dt*1000)
xlabel("Sample [#]")
ylabel("time [ms]")
title("Timed0 execution time")
grid on
subplot(2, 1, 2)
plot(timed2_csv.n_, timed2_csv.dt*1000)
xlabel("Sample [#]")
ylabel("time [ms]")
title("Timed0 execution time")
grid on

%% Distribution
clc
close all

figure
subplot(2, 1, 1)
histogram(timed0_csv.dt*1000, 500)
title("Execution time histogram")
xlabel("Frequency")
ylabel("Value [ms]")
xlim([0 10])
grid on
subplot(2, 1, 2)
histogram(timed2_csv.dt*1000, 500)
title("Execution time histogram")
xlabel("Frequency")
ylabel("Value [ms]")
xlim([0 10])
grid on

%% Get some statistics
print_exe_time_stats(timed0_csv.dt, 'timed0')
print_exe_time_stats(timed2_csv.dt, 'timed2')
