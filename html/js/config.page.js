(function($) {
  'use strict';

  $("#modemSettings input").removeClass("ignore")
  $("#modemSettings select").removeClass("ignore")

  var currentOac = $('#oamodesel :selected').text()

  if (debug) console.log(`current oamodesel | ${currentOac}`);

  if (currentOac.includes('OpenAMIP 9350')) {
    changeOacInputs('OpenAMIP 9350')
  }
  if (currentOac.includes('iDirect 8300 Serial') || currentOac.includes('iDirect 9350 Serial')) {
    changeOacInputs('iDirect 8300 Serial')
  }
  if (currentOac.includes('Newtec')) {
    changeOacInputs('OpenAMIP 9350')
  }

  $('#oamodesel').change(function(){
    var val = $('#oamodesel :selected').text()

    if (debug) console.log(`selected oamodesel | ${val}`);

    if (val.includes('OpenAMIP 9350')) {
      changeOacInputs('OpenAMIP 9350')
    }
    if (val.includes('iDirect 8300 Serial') || val.includes('iDirect 9350 Serial')) {
      changeOacInputs('iDirect 8300 Serial')
    }
    if (val.includes('Newtec')) {
      changeOacInputs('Newtec')
    }

  });

  $("#exportProfileBtn").click(function(e) {

    var formData = $("#modemSettings").serializeArray();

    if ($("#snrthreshold").prop('checked') === false) {
      formData.push({ name: 'snrthreshold', value: 'off'})
      if (debug === true) console.log('snrthreshold set to off');
    }

    // else {
    //   console.log(formData.find(e => e.name === 'snrthreshold').value);
    //   formData.push({ name: 'snrthreshold', value: 'on'})
    // }

    var blob = new Blob([JSON.stringify(formData)], { type: "text/plain;charset=utf-8" });
    var filename = `${currentProfileId}-${profilename}-${new Date().toJSON().slice(0,10)}.json`;

    var isIE = false || !!document.documentMode;
    if (isIE) {
      window.navigator.msSaveBlob(blob, filename);
    } else {
      var url = window.URL || window.webkitURL;
      var link = url.createObjectURL(blob);
      var a = $('<a id="tempDownload" />');
      a.attr('download', filename);
      a.attr('href', link);
      $("body").append(a);
      a[0].click();
      $("#tempDownload").remove;
    }
  });

  $("#importProfileBtn").click(function(e) {
    var myFile = $('#importProfileFile').prop('files')[0];
    var fr = new FileReader();
    var __clone = $("form").clone();
    fr.onload = function() {
      var jsonImportData = JSON.parse(fr.result);
      const processFileImportData = async () => {
        await asyncForEach(jsonImportData, async (item, i) => {
          if (debug) console.log(`Importing | ${item.name}=${item.value}`)
          $(`#${item.name}`).val(item.value)
          if($(`#${item.name}`).is("select")) {
            $(`#${item.name} option[value="${item.value}"]`).prop("selected", true)
            if (item.name === 'oamodesel') {
              changeOacInputs(item.value.replace('>', ''))
            }
          }
          if($(`#${item.name}`).is(":checkbox") && item.value === 'on') {
            $(`#${item.name}`).prop('checked', true);
          } else {
            $(`#${item.name}`).prop('checked', false);
          }
        });

        if ($("form").serialize() != $(__clone).serialize()) {
          $('#saveNotification').removeAttr("hidden");
        } else {
          $('#saveNotification').attr("hidden");
        }
      }
      processFileImportData()
    };
    fr.readAsText(myFile);
  });

  $('#importProfileFile').change(
    function() {
      $('#importProfileBtn').attr('disabled', ($(this).val() ? false : true));
    }
  );

})(jQuery);
