(function($) {
  'use strict';

  $.validator.setDefaults({
    // submitHandler: function() {
    //   reboot();
    // }
  });
  $.validator.addMethod('IP4Checker', function(value) {
    return value.match(/^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$/);
  }, 'Invalid IP address.');

  $.validator.addMethod("twoDecimals", function(value, element) {
    return this.optional(element) || /^-?\d{0,4}(\.\d{0,2})?$/i.test(value);
  }, "You must have a percision at most two decimal places.");

  $.validator.addMethod("fileName", function(value, element) {
    return this.optional(element) || /^[a-z\d\-_\s]+$/i.test(value) }, "Letters, numbers, underscores, and dashes only please")

  $.validator.addMethod("passwordCheck",
    function(value, element) {
       return /^[A-Za-z0-9\d=!\-@._*]+$/.test(value);
    });

  var __clone = $("form").clone();

  $("form")
    .on("input", function() {
      if ($("form").serialize() != $(__clone).serialize()) {
        $('#saveNotification').removeAttr("hidden");
      } else {
        $('#saveNotification').attr("hidden");
      }
    });

  $(function() {
    $("#modemSettings").validate({
      ignore: ".ignore",
      rules: {
        modemip: {
          IP4Checker: true
        },
        modemport: {
          required: true,
          number: true,
          range: [1024, 49151],
        },
        modemroot: {
          required: true,
          minlength: 2
        },
        modemuser: {
          required: true,
          minlength: 2,
          maxlength: 25
        },
        modempass: {
          required: true,
          minlength: 2,
          maxlength: 25,
          passwordCheck: true
        },
        modemsat: {
          required: true,
          number: true,
          range: [-360, 360.99],
          twoDecimals: true
        },
        vscan: {
          required: true,
          number: true,
          range: [0.20, 20.00],
          twoDecimals: true
        },
        trackgain: {
          required: true,
          number: true,
          range: [0, 9999]
        },
        stepadjust: {
          required: true,
          number: true,
          range: [0, 99]
        },
        stepignore: {
          required: true,
          number: true,
          range: [0, 99]
        },
        lockthresh: {
          required: true,
          number: true,
          range: [1, 20],
          twoDecimals: true
        },
        snrhigh: {
          required: true,
          number: true,
          range: [0, 20.00],
          twoDecimals: true
        },
        snrmin: {
          required: true,
          number: true,
          range: [0, 20.00],
          twoDecimals: true
        },
        profilename: {
          fileName: true
        }
      },
      errorPlacement: function(label, element) {
        label.addClass('mt-2 text-danger');
        label.insertAfter(element);
      },
      highlight: function(element, errorClass) {
        $(element).parent().addClass('has-danger')
        $(element).addClass('form-control-danger')
      },
      submitHandler: function(form, e) {
        e.preventDefault();
        var formData = $("#modemSettings").serialize();

        if ($("#snrthreshold").prop('checked') === false) {
          formData = formData + '&snrthreshold=off'
          if (debug === true) console.log('snrthreshold set to off');
        }

        if (debug === true) console.log('modemSettings form submitted');
        $.ajax({
          type: 'POST',
          url: '../FORMADMIN.HTM',
          data: formData,
          error: function(error) {
            console.error(error);
          }
        });
        reboot()

        return false;
      }
    });
  });

  $(function() {
    $("#deviceNetwork").validate({
      rules: {
        oacip: {
          required: true,
          IP4Checker: true
        },
        devicenetmask: "required",
        devicegateway: {
          required: true,
          IP4Checker: true
        },
        devicedns: {
          required: true,
          IP4Checker: true
        }
      },
      errorPlacement: function(label, element) {
        label.addClass('mt-2 text-danger');
        label.insertAfter(element);
      },
      highlight: function(element, errorClass) {
        $(element).parent().addClass('has-danger')
        $(element).addClass('form-control-danger')
      },
      submitHandler: function(form, e) {
        e.preventDefault();
        if (debug === true) console.log('deviceNetwork form submitted');
        $.ajax({
          type: 'POST',
          url: '../FORMPOST.HTM',
          data: $('#deviceNetwork').serialize(),
          // success: function(result) {
          //   reboot()
          // },
          error: function(error) {
            console.error(error);
          }
        });

        reboot()

        return false;
      }
    });
  });

  $(function() {
    $("#changePassowrd").validate({
      rules: {
        oldpassword: {
          required: true,
          minlength: 2
        },
        newpassword: {
          required: true,
          minlength: 5
        },
        confirmpassword: {
          required: true,
          minlength: 5,
          equalTo: "#newpassword"
        },
      },
      errorPlacement: function(label, element) {
        label.addClass('mt-2 text-danger');
        label.insertAfter(element);
      },
      highlight: function(element, errorClass) {
        $(element).parent().addClass('has-danger')
        $(element).addClass('form-control-danger')
      },
      submitHandler: function(form, e) {
        e.preventDefault();
        if (debug === true) console.log('changePassowrd form submitted');
        $.ajax({
          type: 'POST',
          url: '../FORMPOST.HTM',
          data: $('#changePassword').serialize(),
          success: function(result) {
            reboot()
          },
          error: function(error) {
            console.error(error);
          }
        });
        reboot()
        return false;
      }
    });
  });

  $(function() {
    $("#updateFirmware").validate({
      rules: {
        userfile1: "required"
      },
      errorPlacement: function(label, element) {
        label.addClass('mt-2 text-danger');
        label.insertAfter(element);
      },
      highlight: function(element, errorClass) {
        $(element).parent().addClass('has-danger')
        $(element).addClass('form-control-danger')
      },
      submitHandler: function(form, e) {
        var fileData = new FormData().append
        e.preventDefault();
        if (debug === true) console.log('updateFirmware form submitted');
        $.ajax({
          type: 'POST',
          url: '../FILEPOST.HTM',
          async: false,
          cache: false,
          contentType: false,
          processData: false,
          data: new FormData($("#updateFirmware")[0]),
          // success: function(result) {
          //   reboot()
          // },
          error: function(error) {
            console.error(error);
          }
        });
        return false;
      }
    });
  });
})(jQuery);
