clear, clc, close all
data = load('data.mat');
data = data.y4;
data_length = length(data);

data = diff(data,2);

% ��̬�Լ���
[A,B,u,r] = nixujianyan(data(1:720),20);

sk = skewness(data); % ƫ��
ku = kurtosis(data); % ���

data = data - mean(data);

% for i = 0:10
%     for j = 0:10
%         %        2012b֮ǰ
%         %        spec = garchset('R',i,'M',j,'Display','off'); % ָ��ģ�͵Ľṹ
%         %        [coeffX,errorsX,LLFX]=garchfit(spec,x); % ��ϲ���
%         %        num=garchcount(coeffX); % ������ϲ����ĸ���
%         Mdl = arima(i,0,j);
%         [EstMdl,EstParamCov,LLFX] = estimate(Mdl, data');
%         num = sum(any(EstParamCov));
%         % ����Akaike��bayesian��Ϣ׼��
%         [aic,bic]=aicbic(LLFX, num, data_length);
%         fprintf('R=%d, M=%d, AIC=%f, BIC=%f\n',i,j,aic,bic); %��ʾ������
%         aics(i+1,j+1)=aic;
%         bics(i+1,j+1)=bic;
%     end
% end

Mdl = arima(7,0,4);
[EstMdl,EstParamCov,LLFX] = estimate(Mdl, data');
num = sum(any(EstParamCov));
e(1)=0;
for j=2:data_length
    e(j) = data(j) - cell2mat(EstMdl.AR)*e(j-1)-cell2mat(EstMdl.MA)*e(j-1); % ����в�
end

%�������chi2���飬�Ƿ����0��ֵ����̬�ֲ���nparam��ʾ���Ʋ����ĸ���
[h,p,stats] = chi2gof(e,'cdf',@(z)normcdf(z,0,std(e)),'nparams',1);

