clear, clc, close all
% xlsdata = xlsread('data.xlsx',1);
% data = xlsdata(:,2);
data = load('data.mat');
data = data.y4;
data_length = length(data);

% ƽ������
avg = mean(data);
avg_my = sum(data)/data_length; %matlab��֤

% ���ֵ��
data = data - avg;

% ����
gama0 = var(data);

% kȡֵ
L=50;

% ��Э�����
for i = 1:L
    gama1(i) = data(i+1:end)*data(1:end-i)'/data_length; % ��ƫ����
    gama2(i) = data(i+1:end)*data(1:end-i)'/(data_length-L); % ��ƫ����
end

% �����ϵ������
rho1 = gama1 / gama0;
rho2 = gama2 / gama0;
autocorr(data, L); %matlab��֤

figure;
plot([1 rho1],'o')

% ƫ��غ���
f(1,1) = rho1(1);
for k = 2:L
    s1 = rho1(k);
    s2 = 1;
    for j = i:k-1
        s1 = s1 - rho1(k-j)*f(k-1,j);
        s2 = s2 - rho1(j)*f(k-1,j);
    end
    f(k,k) = s1 / s2;
    for j = 1:k-1
        f(k,j) = f(k-1,j)-f(k,k)*f(k-1,k-j);
    end
end

pcorr = diag(f)'; % ƫ���ϵ��

figure;
parcorr(data, L);%  matlab��֤
figure;
plot(pcorr);


