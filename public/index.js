const form = document.getElementById("contactForm");
const resultMsg = document.getElementById("resultMsg");
form.addEventListener("submit", function (e) {
  e.preventDefault();
      
  resultMsg.style.display = "block";
  resultMsg.style.color = "green";
  resultMsg.innerText = "Form submitted successfully!";

  form.reset();
});
 
