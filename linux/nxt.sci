if isdef('nxt_sci') then 
  return
end
nxt_sci=1;

if isdef('lusb') then 
  unlink()
end


lusb=link('libusb.so');
functions=['comm_open','nxt_playtone','nxt_batterylevel', ...
  'nxt_motor_setforward', 'nxt_motor_setreverse', 'nxt_motor_stop', ...
  'nxt_motor_resetrotation', 'nxt_motor_getrotation' ...
  ];
llego=link('legolinux.o',functions,"c");

function unlink()
  ulink(lusb)
  ulink(llego)
  
endfunction

function l()
  link('show')
endfunction


function status=nxt_init()
  status=call('comm_open','out',[1,1],1,'i')
endfunction

function level=nxt_batlevel()
  level=call('nxt_batterylevel','out',[1,1],1,'i');
endfunction

function nxt_playtone(frequency,duration)
    call('nxt_playtone',frequency,1,'i',duration,2,'i','out',[1,1],1,'i');
endfunction


function nxt_forward(port,power)
  call('nxt_motor_setforward',port,1,'i',power,2,'i','out',[1,1],1,'i');
endfunction

function nxt_reverse(port,power)
  call('nxt_motor_setreverse',port,1,'i',power,2,'i','out',[1,1],1,'i');
endfunction

function nxt_stop(port,brake)
  call('nxt_motor_stop',port,1,'i',brake,2,'i','out',[1,1],1,'i');
endfunction

function nxt_resetrotation(port,relative)
  call('nxt_motor_resetrotation',port,1,'i',relative,2,'i','out',[1,1],1,'i');
endfunction

function tacho=nxt_getrotation(port)
  tacho=call('nxt_motor_getrotation',port,1,'i','out',[1,1],2,'i');
endfunction

