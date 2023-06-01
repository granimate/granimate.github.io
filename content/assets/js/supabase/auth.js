var SUPABASE_URL = 'https://xfhukaikvcqbculqorow.supabase.co';
var SUPABASE_KEY = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InhmaHVrYWlrdmNxYmN1bHFvcm93Iiwicm9sZSI6ImFub24iLCJpYXQiOjE2ODU1MTEyNjYsImV4cCI6MjAwMTA4NzI2Nn0.xkkae626Q46O5-BnL6_Qldfd1GkVy_uq1bL6kBptAkU';

var supabase = supabase.createClient(SUPABASE_URL, SUPABASE_KEY);
window.userToken = null;

function signUp() {
  const email = document.getElementById("input-email").value;
  const password = document.getElementById("input-password").value;
  const confirm = document.getElementById("input-confirm-password").value;

  if(password == confirm) {
    supabase.auth
    .signUp({ email, password })
    .then((response) => {
      if(response.error) {
        alert(response.error.message)
      }
      else {
        alert('Confirmation Email Sent');
        window.location.href = "@pathto(auth/sign-in/)";
      }
    })
    .catch((err) => {
      alert(err);
    });
  }
}

function signIn() {
  const email = document.getElementById("input-email").value;
  const password = document.getElementById("input-password").value;

  supabase.auth
    .signInWithPassword({ email, password })
    .then((response) => {
      if(response.error) {
        alert(response.error.message)
      }
      else {
        setToken(response);
        window.location.href = "@pathto(animate/)";
      }
    })
    .catch((err) => {
      alert(err);
    });
}

function forgotPassword() {
  const email = document.getElementById("input-email").value;
  
  supabase.auth.resetPasswordForEmail(email, {
    redirectTo: 'http://localhost:32006/auth/update-password/',
  })

  alert("Password reset email sent");
}

async function updatePassword() {
  const password = document.getElementById("input-password").value;
  const confirm = document.getElementById("input-confirm-password").value;

  if(password == confirm) {
    supabase
      .auth
      .updateUser({password: password})
      .then((response) => {
      if(response.error) {
        alert(response.error.message)
      }
      else {
        alert("Password changed successfully");
        window.location.href = "@pathto(auth/sign-in/)";
      }
    })
    .catch((err) => {
      alert(err);
    });
  }
  else {
    alert("passwords do not match");
  }
}

var access_token, refresh_token;

function signOut() {
  supabase.auth
    .signOut()
    .then((_response) => {
      access_token = refresh_token = '';
      document.getElementById("sign-up-link").style.display = "inline";
      document.getElementById("sign-in-link").style.display = "inline";
      document.getElementById("sign-out-button").style.display = "none";
      document.getElementById("details-link").style.display = "none";
      window.location.href = "@pathto(/)";
    })
    .catch((err) => {
      alert(err.response.text)
    })
}

function setToken(response) {
  access_token = response.data.session.access_token
  refresh_token = response.data.session.refresh_token

  document.getElementById("sign-up-link").style.display = "none";
  document.getElementById("sign-in-link").style.display = "none";
  document.getElementById("sign-out-button").style.display = "inline";
  document.getElementById("details-link").style.display = "inline";

  //alert('Logged in as ' + response.user.email)
}

var sesh;

function fetchUserDetails() {
  alert(JSON.stringify(sesh["data"]["session"]));
}

//alert(JSON.stringify(supabase.auth.user()));
async function init() {
  sesh = await supabase.auth.getSession();

  if(sesh["data"]["session"]) {
    document.getElementById("sign-up-link").style.display = "none";
    document.getElementById("sign-in-link").style.display = "none";
    document.getElementById("sign-out-button").style.display = "inline";
    document.getElementById("details-link").style.display = "inline";

    if(document.getElementById("auth-details"))
      document.getElementById("auth-details").value = JSON.stringify(sesh["data"]["session"], null, "  ");
  }
  else {
    document.getElementById("sign-up-link").style.display = "inline";
    document.getElementById("sign-in-link").style.display = "inline";
    document.getElementById("sign-out-button").style.display = "none";
    document.getElementById("details-link").style.display = "none";

    if(document.getElementById("auth-details"))
      document.getElementById("auth-details").value = "signed out";
    else if(document.getElementById("div-upload"))
      window.location.href = "@pathto(auth/sign-in/)";
  }
}
init();

