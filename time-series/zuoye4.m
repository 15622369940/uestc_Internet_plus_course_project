clear, clc, close all
data = load('data.mat');
data = data.y4;
data_length = length(data);

data = diff(data,2);

% ��̬�Լ���
[A,B,u,r] = nixujianyan(data(1:720),20); 

sk = skewness(data); % ƫ��
ku = kurtosis(data); % ���

