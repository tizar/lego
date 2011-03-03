
link('lego.dll',['nxtinit','nxtstop','getbatterylevel','playtone','playsoundfile','motor_setforward','motor_setreverse','motor_stop','motor_resetrotation','motor_getrotation'],'c')

//call('playtone',1,440,'i',2,1000,'i','out',[1,1],3,'i') ;
function status=nxt_init()
  status=call('nxtinit',1,1,'i','out',[1,1],1,'i')
endfunction

function level=nxt_batlevel()
  level=call('getbatterylevel','out',[1,1],1,'i');
endfunction

function nxt_playtone(frequency,duration)
    call('playtone',frequency,1,'i',duration,2,'i','out',[1,1],1,'i');
endfunction

function nxt_forward(port,power)
  call('motor_setforward',port,1,'i',power,2,'i','out',[1,1],1,'i');
endfunction

function nxt_reverse(port,power)
  call('motor_setreverse',port,1,'i',power,2,'i','out',[1,1],1,'i');
endfunction

function nxt_stop(port,brake)
  call('motor_stop',port,1,'i',brake,2,'i','out',[1,1],1,'i');
endfunction

function nxt_resetrotation(port,relative)
  call('motor_resetrotation',port,1,'i',relative,2,'i','out',[1,1],1,'i');
endfunction

function rotation=nxt_getrotation(port)
  rotation=call('motor_getrotation',port,1,'i','out',[1,1],2,'i');
endfunction
