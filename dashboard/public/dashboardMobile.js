var serverURL = 'obd.today:3333';
var dashboardStatus;
var lightsOnCnt = 0;

function formatDate(date) {
    return date.getDate() + "/" + (date.getMonth()+1) + " " + date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
}

function updateTemperaturesCaption() {
    var Tin = dashboardStatus.temperatures['predspalnou'].value    ;
    var Tout = dashboardStatus.temperatures['balkon'].value;
    if (Tin === undefined) Tin = NaN;
    if (Tout === undefined) Tout = NaN;
    $('div#temperatures h3>a').text('Teploty -  ' + Tout + ' / ' + Tin);    
}

function updateHeatingCaption() {
    Hthermostats = '';
    for (var tht in dashboardStatus.thermostats) {
        Hthermostats += dashboardStatus.thermostats[tht].value + ',';       
    }
    
    $('div#heating h3>a').text('Kúrenie - ' + dashboardStatus.heating.value + ' (' + Hthermostats.replace(/,$/, ')'));    
}

function updateLightsCaption() {
    if (lightsOnCnt < 0) lightsOnCnt = 0;
    if (lightsOnCnt == 0) $('div#lights h3>a').text('Svetá - All OFF');
    else $('div#lights h3>a').text('Svetá - ' + lightsOnCnt + ' ON');    
}



$.get('http://' + serverURL + '/status', null, function(data) {
    dashboardStatus = data;
    ligthsOnCnt = 0;
    for (var lgt in dashboardStatus.lights) {        
        $('div#lights select[name="'+ lgt + '"]').val(dashboardStatus.lights[lgt].value);
        if (dashboardStatus.lights[lgt].value == 'ON') { lightsOnCnt ++; }
        $('div#lights label[name="L'+ lgt + 'Time"]').text(formatDate(new Date(dashboardStatus.lights[lgt].time)));
        $('div#lights select[name="'+ lgt + '"]').flipswitch("refresh");
    }
    updateLightsCaption();
    
    for (var tmp in dashboardStatus.temperatures) {        
        $('div#temperatures td[name="'+ tmp + '"]').text(dashboardStatus.temperatures[tmp].value + ' ℃');
        $('div#temperatures td[name="T'+ tmp + 'Time"]').text(formatDate(new Date(dashboardStatus.temperatures[tmp].time)));
    }
    updateTemperaturesCaption();
    
    for (var swc in dashboardStatus.switches) {        
        $('div#switches td[name="'+ swc + '"]').text(dashboardStatus.switches[swc].value);
        $('div#switches td[name="S'+ swc + 'Time"]').text(formatDate(new Date(dashboardStatus.switches[swc].time)));
    }

    for (var rqt in dashboardStatus.requiredTemperatures) {        
        $('div#heating td[name="'+ rqt + '"]').text(dashboardStatus.requiredTemperatures[rqt].value);
        $('div#heating td[name="R'+ rqt + 'Time"]').text(formatDate(new Date(dashboardStatus.requiredTemperatures[rqt].time)));
    }
    
    for (var tht in dashboardStatus.thermostats) {        
        $('div#heating select[name="'+ tht + '"]').val(dashboardStatus.thermostats[tht].value);
        $('div#heating label[name="M'+ tht + 'Time"]').text(formatDate(new Date(dashboardStatus.thermostats[tht].time)));
        $('div#heating select[name="'+ tht + '"]').flipswitch("refresh");
    }
    
    $('div#heating select[name="heating"]').val(dashboardStatus.heating.value)   ;
    $('div#heating label[name="HheatingTime"]').text(formatDate(new Date(dashboardStatus.heating.time)));
    $('div#heating select[name="heating"]').flipswitch("refresh");
    updateHeatingCaption();
    
}, 'json');

$('div#lights select').on( "change", function(event, ui) {
    $.get('http://' + serverURL + '/lights/' + this.name + '/' + $(this).val(), null, function(data) {
    });
    
});

$('div#heating select[name="heating"]').on( "change", function(event, ui) {
    $.get('http://' + serverURL + '/heating/' + $(this).val(), null, function(data) {
    });
});

$('div#heating select[name="byt7"]').on( "change", function(event, ui) {
    $.get('http://' + serverURL + '/thermostats/' + this.name + '/' + $(this).val(), null, function(data) {
    });    
});

$('div#heating select[name="byt8"]').on( "change", function(event, ui) {
    $.get('http://' + serverURL + '/thermostats/' + this.name + '/' + $(this).val(), null, function(data) {
    });
});

var socket = io.connect('ws://' + serverURL);

socket.on('lgtIs', function (data) {
    $('div#lights select[name="'+ data.name + '"]').val(data.value);
    $('div#lights label[name="L'+ data.name + 'Time"]').text(formatDate(new Date(data.time)));
    $('div#lights select[name="'+ data.name + '"]').flipswitch("refresh");
    
    dashboardStatus.lights[data.name].value = data.value;
    dashboardStatus.lights[data.name].time = data.time;
    if (data.value == 'ON') lightsOnCnt ++;
    else lightsOnCnt --;
    
    updateLightsCaption();
});

socket.on('swcIs', function (data) {
        $('div#switches td[name="'+ data.name + '"]').text(data.value);
        $('div#switches td[name="S'+ data.name + 'Time"]').text(formatDate(new Date(data.time)));
});

socket.on('tmpIs', function (data) {
        $('div#temperatures td[name="'+ data.name + '"]').text(data.value + ' ℃');
        $('div#temperatures td[name="T'+ data.name + 'Time"]').text(formatDate(new Date(data.time)));
        dashboardStatus.temperatures[data.name].value = data.value;
        dashboardStatus.temperatures[data.name].time = data.time;
        updateTemperaturesCaption();
});

socket.on('htnIs', function (data) {
    $('div#heating select[name="heating"]').val(data.value);
    $('div#heating label[name="HheatingTime"]').text(formatDate(new Date(data.time)));
    $('div#heating select[name="heating"]').flipswitch("refresh");
    dashboardStatus.heating.value = data.value;
    dashboardStatus.heating.time = data.time;
    updateHeatingCaption();
});

socket.on('rqtIs', function (data) {
    $('div#heating td[name="'+ data.name + '"]').text(data.value);
    $('div#heating td[name="S'+ data.name + 'Time"]').text(formatDate(new Date(data.time)));
    dashboardStatus.requiredTemperatures[data.name].value = data.value;
    dashboardStatus.requiredTemperatures[data.name].time = data.time;
});

socket.on('thtIs', function (data) {
    $('div#heating select[name="'+ data.name + '"]').val(data.value);
    $('div#heating label[name="M'+ data.name + 'Time"]').text(formatDate(new Date(data.time)));
    $('div#heating select[name="'+ data.name + '"]').flipswitch("refresh");
    dashboardStatus.thermostats[data.name].value = data.value;
    dashboardStatus.thermostats[data.name].time = data.time;
    updateHeatingCaption();
});


