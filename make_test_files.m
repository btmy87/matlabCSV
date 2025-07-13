%% make_test_files
% generate larger test files

close all
clear
clc

n = 1e7;
t = table();
t.x = (1:n)';
t.y1 = t.x;
t.y2 = t.x.^2;
t.y3 = sin(t.x/1e3);
t.y4 = cos(t.x/1e3);

writetable(t, "test2.csv");