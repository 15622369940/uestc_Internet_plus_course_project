% y = detrend(x) % ����ʱ�������е�����������
% y = detrend(x,'constant') % ����ʱ�������еľ�ֵ
% y = detrend(x,'linear',bp) % �ֶ�����ʱ�������е�����������,bpΪ�ֶε�����
clc, clear, close all  ;
data = xlsread('data.xlsx', 'Sheet1', 'A2:B732');
x=data(:,1);
y=data(:,2);
n = length(y);

% ԭʼͼ��
figure;
plot(y);title('ԭʼͼ��')

L = 50;

% ԭʼ�����ϵ��
figure;
autocorr(y, L);title('ԭʼ�����ϵ��')

% ȥ������������
y1=detrend(y);
figure;
plot(y1);title('ȥ������������ͼ��')

% �����ϵ��
figure;
autocorr(y1, n-1);title('ȥ�����������������ϵ��')

% ���κ������
p = polyfit(x,y,2);
y3 = polyval(p,x);

% ȥ��������
y3 = y - y3;
figure;
plot(y3);title('ȥ������������ͼ��')

% �����ϵ��
figure;title('ȥ�����������������ϵ��')
autocorr(y3, n-1);

% ȥ��������
T = [1 32	61	92	122	153 183	214	245	275	306	336 367	398	426	457	487	518 548	579	610	640	671	701 732];
T_total = length(T)-1;
for i=1:T_total
    average(i) = mean(y3(T(i):T(i+1)-1));
    y4(T(i):T(i+1)-1) = y3(T(i):T(i+1)-1) - average(i);
end
figure;
plot(y4);title('���ȥ��������ͼ��')

figure;
autocorr(y4, n-1);title('���ȥ��������ͼ��')

save('data.mat', 'y4');

% ���ײ��
% y2 = diff(y,2);
% figure;
% plot(y2);
% n = length(y2);
% [acf,lags] = autocorr(y2, L);
% 
% y3 = diff(y2,31);
% figure;
% plot(y2);
% title('��ֺ�')
% n = length(y3);
% autocorr(y3, L);
% y4 = y3';
% save('data.mat', 'y4');