
close all

% tests a neuron that reproduces Fig 3 in Tim O'Leary's paper

x = xolotl.examples.neurons.BurstingNeuron('prefix','liu');

g0 = 1e-1+1e-1*rand(8,1);
x.set('*gbar',g0);
x.AB.Leak.gbar = .099;

% configure the target
x.AB.add('CalciumTarget','Target',7)


x.AB.NaV.add('oleary/IntegralController','tau_m',666);
x.AB.CaT.add('oleary/IntegralController','tau_m',55555);
x.AB.CaS.add('oleary/IntegralController','tau_m',45454);
x.AB.ACurrent.add('oleary/IntegralController','tau_m',5000);
x.AB.KCa.add('oleary/IntegralController','tau_m',1250);
x.AB.Kd.add('oleary/IntegralController','tau_m',2000);
x.AB.HCurrent.add('oleary/IntegralController','tau_m',125000);


% try to download a binary
% so that people without compilers can use this too
try
	x.download
end

x.t_end = 5e5;
x.sim_dt = .1;
x.dt = 100;
[~,~,C] = x.integrate;



figure('outerposition',[300 300 900 600],'PaperUnits','points','PaperSize',[1200 600]); hold on
subplot(2,1,1); hold on

time = x.dt*(1:length(C))*1e-3;
plot(time,C(:,2:2:end));
set(gca,'XScale','log','YScale','log','YTick',[1e-2 1e0 1e2 1e4])
xlabel('Time (s)')
ylabel('g (uS/mm^2)')

subplot(2,1,2); hold on
x.dt = .1;
x.t_end = 1e3;
V = x.integrate;
time = x.dt*(1:length(V))*1e-3;
plot(time,V,'k')
set(gca,'YLim',[-80 50])
ylabel('V_m (mV)')
xlabel('Time (s)')
drawnow

figlib.pretty('PlotLineWidth',1.5,'LineWidth',1.5)
