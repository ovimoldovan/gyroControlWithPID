m = 0.03; %wheel mass
M = 0.5; %mbot mass
R = 0.03; %wheel radius
Jw = m*(R^2)/2; %wheel inertia moment
n = 1; %gear ratio;
W = 0.133; %body width
D = 0.05; %body depth
H = 0.129; %body height
g = 9.8; %gravity acceleration
L = H/2; %distance of the center of the mass from the wheel axle
Jfork = M*(L^2)/3; %body pitch inertia moment
Jphi = M*((W^2) + (D^2))/12; %body yaw inertia moment
Jm = 1e-5; %dc motor inertia moment
Rm = 6.69; %[ohm], dc motor resistance
Kb = 0.468; % [V sec / rad] dc motor back emf constant
Kt = 0.317; %[Nm/A] dc motor torque constant
fm = 0.0022; %friction coefficient between body and dc motor
fw = 0; % friction coefficient between body and motion surface

e12 = M*L*R - 2*n^2*Jm;
e11 = (2*m + M) * R^2 + 2*Jw + 2*n^2*Jm;
e22 = M*(L^2) + Jfork + 2*n^2*Jm;

detE = e11*e22-(e12^2);

alpha = n*Kt/Rm;
beta = n*Kt*Kb/Rm + fm;
sigma = beta + fw;

a32 = -g*M*L*e12/detE;
a42 = g*M*L*e11/detE;
a33 = -2*(sigma*e22 + beta*e12) / detE;
a43 = 2 * (sigma * e12 + beta * e11)/detE;
a34 = 2 * beta * (e22 + e12) / detE;
a44 = -2 * beta *(e11 + e12) / detE;
b3 = alpha * (e11 + e12) / detE;
b4 = -alpha * (e11 + e12) / detE;

A = [0 0 1 0; 0 0 0 1; 0 a32 a33 a34; 0 a42 a43 a44];
B = [0 0; 0 0; b3 b3; b4 b4];

%Discretizing

Ad = expm(A*0.01);
Bd = A\(Ad - eye(size(Ad))) * B;
eig(A); %poles => unstable

%trial and error; read from paper

PID1 = tf(pid(-77.97, -0.01, -8,79, 0.01));
PID2 = tf(pid(-1.07, -0.01, -1.36, 0.01, 0.01));

