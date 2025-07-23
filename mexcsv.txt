% mexcsv read csv and return in table.init format
% mexcsv(filename, rowVarNames, rowsSkip)
%
% Inputs:
%  filename: 1xN char array, path to csv file
%  rowVarNames: row number (0-based) containing variable names
%  rowsSkip: number of rows to skip after variable names
%
% Outputs:
%   vars: cell array of table columns in table.init format
%   varnames: cell array of variable names