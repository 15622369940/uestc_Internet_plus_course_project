clc,clear, close all;
randn('state',sum(clock));
%��ʼ�������������
elps=randn(1,10000);
%����10000�����ӱ�׼��̬�ֲ��������
x(1)=0;
%����ʼֵ
for j=2:10000
    x(j)=0.8*x(j-1)+elps(j)-0.4*elps(j-1);
    %����������
end

y=(x-mean(x)); %���������Ļ�
gama0=var(x); %����������
L=20;
for j=1:L
    gama(j)=y(j+1:end)*y(1:end-j)'/10000;%����Э����
end
rho=gama/gama0      %�������ϵ��
f(1,1)=rho(1);
for k=2:L
    s1=rho(k);s2=1;    %ʽ��90���еڶ�ʽ����ĳ�ʼֵ
    for  j=1:k-1
        s1=s1-rho(k-j)*f(k-1,j);
        s2=s2-rho(j)*f(k-1,j);
    end
    f(k,k)=s1/s2;%ʽ��90���еڶ�ʽ�ļ���ֵ
    for  j=1:k-1
        f(k,j)=f(k-1,j)-f(k,k)*f(k-1,k-j); %ʽ��90���е���ʽ�ļ���ֵ
    end
end
pcorr=diag(f)';      %��ȡƫ��غ���
parcorr(x,L) %ֱ������Matlab���������ƫ��غ���
figure;
plot(pcorr);
figure;
plot(pcorr2);