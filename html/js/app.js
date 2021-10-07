var debug = true

// local/non-prod settings
var local = false

if (local === true) {
  $('#profileList').html(`"5" value"FACTORY DEFAULTS" value"(spare)" value"(spare)" value"(spare)" value"(spare)" value"(spare)" value"(spare)" value"(spare)" value"(spare)" value"(spare)" value"(spare)"`);
}

var path = window.location.pathname;
var page = path.split("/").pop();
var systemRebootTime = 10000

var start = -9
var stop = 16
var step = 2
var signalBarArray = Array(Math.ceil((stop - start) / step)).fill(start).map((x, y) => x + y * step)

var profileSettingCount = 10
var profileValueArr = $("#profileList").html().trim().replaceAll("value", "").split(/(?!^)".*?"/g).map(function(x) { return x.replace(/"/g, ''); })

var selectedProfileId
var currentProfileId = profileValueArr[0]
var profilename = profileValueArr[profileValueArr[0]]

var uploadCircleProgress = new ProgressBar.Circle(circleProgress2, {
  color: '#aaa',
  from: { color: '#aaa', width: 1 },
  to: { color: '#333', width: 4 },
  strokeWidth: 4,
  trailWidth: 1,
  easing: 'easeInOut',
  text: {
    autoStyleContainer: true,
  },
  from: {
    color: '#aaa',
    width: 4
  },
  to: {
    color: '#677ae4',
    width: 4
  },
  step: function(state, circle) {
    circle.path.setAttribute('stroke', state.color);
    circle.path.setAttribute('stroke-width', state.width);
    var value = Math.round(circle.value() * 100);
    circle.path.setAttribute('stroke', state.color);
    circle.path.setAttribute('stroke-width', state.width);
    circle.setText(value);
  }
});

if (!localStorage.getItem('password')) {
  localStorage.setItem('password', '5fcf82bc15aef42cd3ec93e6d4b51c04df110cf77ee715f62f3f172ff8ed9de9')
}

if (!sessionStorage.getItem('loggedIn')) {
  sessionStorage.setItem('loggedIn', false)
}

function findSignalBarIndex(target) {
  let min;
  let chosen = 0;
  for (let i in signalBarArray) {
    min = Math.abs(signalBarArray[chosen] - target);
    if (Math.abs(signalBarArray[i] - target) < min) {
      chosen = i;
    };
  };
  return chosen;
}

function login() {

  var username = document.getElementById('username').value;
  var password = forge_sha256(document.getElementById('password').value);

  if (password === localStorage.getItem('password') && username === 'admin') {
    if (page === 'admin.html') {
      window.location.assign('admin.html');
    } else {
      window.location.assign('pages/admin.html');
    }
    sessionStorage.setItem('loggedIn', true)
  } else {
    alert('Login Failed');
  }
}

function logout() {
  sessionStorage.setItem('loggedIn', false)
  if (page === 'admin.html' || page === 'config.html') {
    window.location.assign('../index.html');
  } else {
    window.location.assign('index.html');
  }
}

function changePassBtnClick() {

  var oldPassword = forge_sha256(document.getElementById('old-password').value);
  var newPassword = document.getElementById('new-password').value;
  var repeatPassword = document.getElementById('repeat-password').value;

  if (newPassword !== "" && oldPassword === localStorage.getItem('password') && newPassword === repeatPassword) {
    localStorage.setItem('password', forge_sha256(newPassword));
    alert('Password changed');

    document.getElementById('oldpassword').value = '';
    document.getElementById('newpassword').value = '';
    document.getElementById('confirmpassword').value = '';
  } else {
    if (forge_sha256(oldPassword) !== localStorage.getItem('password')) {
      alert('Wrong Password');
    }
    if (newPassword !== repeatPassword) {
      alert('New Password Does Not Match');
    }
  }
}

function showProfileModal(id) {
  selectedProfileId = id
  $('#modalProfileName').text(profileValueArr[id]);
}

function showChangeSettigsModal(id) {
  selectedProfileId = id
  $('#modalProfileName').text(profileValueArr[id]);
}

function changeOacInputs(oacSetting) {

  if (debug) console.log(`switch oamodesel | ${oacSetting}`);
  $("#modemSettings input").removeClass("ignore")
  $("#modemSettings select").removeClass("ignore")

  $('#sattelite-settings').show();
  $('#open-aimp').show();
  $('#serial-telnet-settings').show();
  $('#sattelite-settings').show();

  switch (oacSetting) {
    case 'OpenAMIP 9350':
      $('#sattelite-settings').hide();

      $("#modemsat").addClass("ignore");
      $("#Hor").addClass("ignore");
      $("#Ver").addClass("ignore");
      $("#None").addClass("ignore");
      break;
    case 'iDirect 8300 Serial':
      $('#open-aimp').hide();
      $("#modemip").addClass("ignore");
      $("#modemport").addClass("ignore");
      break;
    case 'Newtec':

      $('#serial-telnet-settings').hide();

      $("#serialspeed").addClass("ignore");
      $("#modemroot").addClass("ignore");
      $("#modemuser").addClass("ignore");
      $("#modempass").addClass("ignore");

      $('#sattelite-settings').hide();

      $("#modemsat").addClass("ignore");
      $("#Hor").addClass("ignore");
      $("#Ver").addClass("ignore");
      $("#None").addClass("ignore");
      break;
    default:
  }
};

function reboot() {
  $('#rebootModal').modal("show");
  var bar = new ProgressBar.Circle(circleProgress1, {
    color: '#aaa',
    from: { color: '#aaa', width: 1 },
    to: { color: '#333', width: 4 },
    strokeWidth: 4,
    trailWidth: 1,
    easing: 'easeInOut',
    duration: systemRebootTime,
    text: {
      autoStyleContainer: true
    },
    from: {
      color: '#aaa',
      width: 4
    },
    to: {
      color: '#677ae4',
      width: 4
    },
    step: function(state, circle) {
      circle.path.setAttribute('stroke', state.color);
      circle.path.setAttribute('stroke-width', state.width);
      var value = Math.round(circle.value() * 100);
      circle.path.setAttribute('stroke', state.color);
      circle.path.setAttribute('stroke-width', state.width);

      if (value === 0) {
        circle.setText('0');
      } else {
        circle.setText(value);
      }
    }
  });

  bar.animate(1.0, function() {
    window.location.assign(page);
  });
}

async function asyncForEach(array, callback) {
  for (let index = 0; index < array.length; index++) {
    await callback(array[index], index, array);
  }
}

(function($) {
  'use strict';

  // dropify
  $('.dropify').dropify();

  var formPostApi = "../FORMPOST.HTM";
  var actionButtonPostApi = "../BUTTONPOST.HTM";
  var profilePostApi = "../LOADPROFILE.HTM";
  var pausePostApi = "../PAUSEPOST.HTM";

  // current profile display
  $('#currentProfile').html(`Current Profile: ${profilename}`)

  // profile handling
  const restoreProfileSelect = document.querySelector("#profileSettingDropdown")

  for (var i = 1; i <= profileSettingCount; i++) {
    let aLinkSelect = `<a id="profileSettingSelect-${i}" class="dropdown-item font-weight-medium" data-toggle="modal" data-target="#loadProfileModal" onclick="showProfileModal(${i})">${profileValueArr[i]}</a>`

    restoreProfileSelect.insertAdjacentHTML("beforeend", aLinkSelect)

    if (i == currentProfileId) {
      $(`#profileSettingSelect-${i}`).addClass('profile-slected');
    }
  }

  $('#currentProfileId').val(`#${currentProfileId}`);
  $('#profilename').val(profilename);

  $("#submitLoadProfile").click(function(e) {
    $.post(profilePostApi, `changeProfile=${selectedProfileId}&systemReboot=${true}`).done(function() {
        console.log("submit-profile success");
      })
      .fail(function() {
        console.error(`changeprofile-post ${selectedProfileId} error`);
      })
    $('#loadProfileModal').modal("hide");
    reboot()
  });

  $("#cancelLoadProfile").click(function(e) {
    $("#loadProfileSelect").val(currentProfileId).change();
    $('#loadProfileModal').modal("hide");
  });

  $("#loadChangeSettingsModalBtn").click(function(e) {
    if ($("#modemSettings").valid()) {
      $('#loadChangeSettingsModal').modal("show");
    }
  });

  $("#submitChangeSettings").click(function(e) {
    $("#modemSettings").submit();
    $('#loadChangeSettingsModal').modal("hide");
  });

  $("#cancelChangeSettings").click(function(e) {
    $('#loadChangeSettingsModal').modal("hide");
  });

  // action button commands
  $("#run-button").click(function(e) {
    $.post(actionButtonPostApi, "buttonPressed=Run").done(function() {
        console.log("run-button success");
      })
      .fail(function() {
        console.error("run-button error");
      })
  });

  $("#stop-button").click(function(e) {
    $.post(actionButtonPostApi, "buttonPressed=Stop").done(function() {
        console.log("stop-button success");
      })
      .fail(function() {
        console.error("stop-button error");
      })
  });

  $("#stow-button").click(function(e) {
    $.post(actionButtonPostApi, "buttonPressed=Stow").done(function() {
        console.log("stow-button success");
      })
      .fail(function() {
        console.error("stow-button error");
      })
  });

  $("#tracstar-functions button").click(function(e) {
    $('#tracstar-functions .active').removeClass('active');
  });

  // system pause handling
  $("#systemPauseModalBtn").click(function(e) {
    $('#systemPauseModal').modal("show");
  });

  $("#systemPause").click(function(e) {
    $('#systemPauseModal').modal("hide");
    $.post(pausePostApi, "buttonPaused=True, systemReboot=True").done(function() {
        console.log("run-button success");
      })
      .fail(function() {
        console.error("run-button error");
      })
    reboot()
  });

  $("#cancelSystemPause").click(function(e) {
    $('#systemPauseModal').modal("hide");
  });

  $("#submitUpdateModalBtn").click(function(e) {
    $('#updateFirmwareModal').modal("show");
  });

  $("#updateFirmwareBtn").click(function(e) {
    $('#updateFirmwareModal').modal("hide");
    $("#updateFirmware").submit()
  });

  $("#updateFirmware").on('submit', function(e) {

    var formData = new FormData(this);
    formData.append("rebootCheckbox", "true");
    e.preventDefault();

    $.ajax({
      type: 'POST',
      url: '../FILEPOST.HTM',
      data: formData,
      contentType: false,
      cache: false,
      processData: false,
      beforeSend: function() {
        $('#uploadModal').modal("show")
        uploadCircleProgress.animate(1)
      },
      success: function(response) {
        if (debug) console.log(response)
        $('#uploadModal').modal("hide");

        reboot()
      },
      error: function(error) {
        console.error(error)
      },
      xhr: function() {
        var xhr = new window.XMLHttpRequest()
        xhr.upload.addEventListener("progress", function(event) {
          if (debug) console.log(event)
          if (event.lengthComputable) {
            var progress = Math.round((event.loaded / event.total) * 100)
            if (debug) console.log(progress)
            uploadCircleProgress.set(progress / 100)
          }
        }, false);
        return xhr;
      }
    });
  });

  $("#cancelUpdateFirmwareBtn").click(function(e) {
    $('#updateFirmwareModal').modal("hide");
  });

  $('#updateFirmwareFile').change(
    function() {
      $('#submitUpdateModalBtn').attr('disabled', ($(this).val() ? false : true));
    }
  );

  // auth handling
  if (sessionStorage.getItem('loggedIn') === 'true') {
    $("#admin-profile").show();
    $(".admin-nav-link").show();
    $("#login-button").hide();
  } else {
    $("#admin-profile").hide();
    $(".admin-nav-link").hide();
    $("#login-button").show();
  }

  // websocket
  var ws = local === true ? new WebSocket("ws://127.0.0.1:8080/ws") : new WebSocket("ws://" + window.location.hostname + "/ws");

  ws.onopen = function(event) {
    ws.send("Here's some text that the server is urgently awaiting!");
  };

  ws.onmessage = function(event) {
    var statusJSON = JSON.parse(event.data)

    if (debug) {
      console.log(event);
      console.log(`clinet recieved: ${event.data}`);
      console.log(`Message Object: parsed`);
      console.log(statusJSON);
    }

    if ('IDD' in statusJSON.status) {
      $('#display-1').val(statusJSON.status.IDD.substring(0, 16))
      $('#display-2').val(statusJSON.status.IDD.substring(17, 33))
      ws.send("exchData->snr");
      if (debug) {
        console.log(`Message Object | IDD | ${statusJSON.status.IDD}`);
        console.log(`Message object | IDD | Line 1 00-16 chars | ${statusJSON.status.IDD.substring(0, 16)}`);
        console.log(`Message object | IDD | Line 2 17-33 chars | ${statusJSON.status.IDD.substring(17, 33)}`);
        console.log('Reply to WSS | IDD Message Recieved and Display Updated');
        ws.send("IDD Message Recieved and Display Updated");
      }
    }

    if ('SNR' in statusJSON.status) {

      $('#signal-strength-value').html(statusJSON.status.SNR)
      $('.br-theme-bars-square .br-widget a').css('border-color', 'rgba(245, 166, 35, 0.6)')

      if (debug) {
        console.log(`Message object | SNR | ${statusJSON.status.SNR}`);
      }

      if (statusJSON.status.SNR >= -9) {
        $('.br-theme-bars-square .br-widget a').css('background-color', 'lightgreen')
        $('select').barrating('set', +findSignalBarIndex(statusJSON.status.SNR) + 1);
        $('.br-theme-bars-square .br-widget a.br-active, .br-theme-bars-square .br-widget a.br-selected').css('background-color', 'green')
        $('.br-theme-bars-square .br-widget a.br-active, .br-theme-bars-square .br-widget a.br-selected').css('border-color', 'green')
      } else {
        $('.br-theme-bars-square .br-widget a').css('background-color', 'red')
        $('select').barrating('set', 0);
      }
    }

    if ('ANT' in statusJSON.status) {
      statusJSON.status.ANT == 1 ? $('#antenna-status').css('background-color', 'green') : $('#antenna-status').css('background-color', 'red')

      if (debug) {
        console.log(`Message object | ANT | ${statusJSON.status.ANT}`);
      }

    }

    if ('MOD' in statusJSON.status) {
      statusJSON.status.MOD == 1 ? $('#modem-status').css('background-color', 'green') : $('#modem-status').css('background-color', 'red')

      if (debug) {
        console.log(`Message object | MOD | ${statusJSON.status.MOD}`);
      }

    }

  }

})(jQuery);
