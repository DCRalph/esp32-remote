#pragma once

#include <pgmspace.h>

const char keypad_js[] PROGMEM = R"rawliteral(class Keypad{constructor(t){this.process=t,this.code=[],this.outterDiv=document.createElement("div"),this.innerDiv=document.createElement("div"),this.displayDiv=document.createElement("div"),this.display=document.createElement("span"),this.progressDiv=document.createElement("div"),this.btns=[],this.btn0=document.createElement("button"),this.btn1=document.createElement("button"),this.btn2=document.createElement("button"),this.btn3=document.createElement("button"),this.btn4=document.createElement("button"),this.btn5=document.createElement("button"),this.btn6=document.createElement("button"),this.btn7=document.createElement("button"),this.btn8=document.createElement("button"),this.btn9=document.createElement("button"),this.btnS=document.createElement("button"),this.btnC=document.createElement("button"),this.btn0.innerHTML="0",this.btn1.innerHTML="1",this.btn2.innerHTML="2",this.btn3.innerHTML="3",this.btn4.innerHTML="4",this.btn5.innerHTML="5",this.btn6.innerHTML="6",this.btn7.innerHTML="7",this.btn8.innerHTML="8",this.btn9.innerHTML="9",this.btnS.innerHTML=`<svg
    xmlns="http://www.w3.org/2000/svg"
    class="h-16 w-16"
    fill="none"
    viewBox="0 0 24 24"
    stroke="currentColor"
  >
    <path
      stroke-linecap="round"
      stroke-linejoin="round"
      stroke-width="2"
      d="M5 13l4 4L19 7"
    />
  </svg>`,this.btnC.innerHTML=`<svg
    xmlns="http://www.w3.org/2000/svg"
    class="h-16 w-16"
    fill="none"
    viewBox="0 0 24 24"
    stroke="currentColor"
  >
    <path
      stroke-linecap="round"
      stroke-linejoin="round"
      stroke-width="2"
      d="M6 18L18 6M6 6l12 12"
    />
  </svg>`,this.btns.push(this.btn0),this.btns.push(this.btn1),this.btns.push(this.btn2),this.btns.push(this.btn3),this.btns.push(this.btn4),this.btns.push(this.btn5),this.btns.push(this.btn6),this.btns.push(this.btn7),this.btns.push(this.btn8),this.btns.push(this.btn9),this.btns.push(this.btnS),this.btns.push(this.btnC),this.outterDiv.classList.add("h-full","w-full","flex","fixed","top-0","inset-0","items-center","justify-center","z-50","bg-gray-900/50"),this.innerDiv.classList.add("grid","grid-cols-3","bg-gray-800","rounded-lg","p-4","gap-4","shadow","transform","select-none"),this.displayDiv.classList.add("bg-gray-500","col-span-3","rounded-lg","mb-1","w-full","h-24","flex","items-center","py-2","text-5xl","relative"),this.display.classList.add("flex","w-full","justify-center"),this.progressDiv.classList.add("absolute","rounded-lg","bottom-0","h-2","transition-all","duration-500","w-0"),this.btn0.classList.add("btn","btn-gray"),this.btn1.classList.add("btn","btn-gray"),this.btn2.classList.add("btn","btn-gray"),this.btn3.classList.add("btn","btn-gray"),this.btn4.classList.add("btn","btn-gray"),this.btn5.classList.add("btn","btn-gray"),this.btn6.classList.add("btn","btn-gray"),this.btn7.classList.add("btn","btn-gray"),this.btn8.classList.add("btn","btn-gray"),this.btn9.classList.add("btn","btn-gray"),this.btnS.classList.add("btn","btn-green"),this.btnC.classList.add("btn","btn-red"),this.displayDiv.appendChild(this.display),this.displayDiv.appendChild(this.progressDiv),this.innerDiv.appendChild(this.displayDiv),this.innerDiv.appendChild(this.btn1),this.innerDiv.appendChild(this.btn2),this.innerDiv.appendChild(this.btn3),this.innerDiv.appendChild(this.btn4),this.innerDiv.appendChild(this.btn5),this.innerDiv.appendChild(this.btn6),this.innerDiv.appendChild(this.btn7),this.innerDiv.appendChild(this.btn8),this.innerDiv.appendChild(this.btn9),this.innerDiv.appendChild(this.btnS),this.innerDiv.appendChild(this.btn0),this.innerDiv.appendChild(this.btnC),this.outterDiv.appendChild(this.innerDiv),this.btns.forEach(t=>{t==this.btnS?t.addEventListener("click",this.submit.bind(this)):t==this.btnC?t.addEventListener("click",this.clear.bind(this)):t.addEventListener("click",this.add.bind(this))}),this.reset()}create=t=>{this.parent=t,t.appendChild(this.outterDiv)};destroy=()=>{this.parent.removeChild(this.outterDiv)};spinner=`<svg class="animate-spin h-24 w-24 p-4" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
				<circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
				<path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
			</svg>`;progressBar=(t,s)=>{this.progressDiv.classList.forEach(t=>{t.includes("bg-")&&this.progressDiv.classList.remove(t)}),this.progressDiv.classList.add(t),this.progressDiv.style.width=`${s}%`};msg=(t,s="text-white")=>{this.display.innerHTML=`<span class="${s} font-bold">
							   ${t}
						   </span>`};disableButtons=()=>{this.btns.forEach(t=>{t.setAttribute("disabled","")})};enableButtons=()=>{this.btns.forEach(t=>{t.removeAttribute("disabled")})};add=t=>{this.code.length<10&&(this.code.push(t.target.innerText),this.msg(this.code.join("")))};reset=()=>{this.code=[],this.msg("Enter code"),this.enableButtons(),this.progressBar("bg-white",0)};clear=()=>{this.reset()};submit=()=>{this.disableButtons(),this.msg(this.spinner,"text-white"),this.progressBar("bg-white",25);let t=()=>{this.msg("Success","text-green-500"),this.progressBar("bg-green-500",100)},s=()=>{this.msg("Fail","text-red-500"),this.progressBar("bg-red-500",100)};if(this.code.length>0&&this.code.length<=10){let i=!1;new Promise((t,s)=>{setTimeout(()=>{s()},5e3),this.process(this.code,t,s)}).then(s=>{i=!0,"object"==typeof s?(this.msg(s.text,`${s.classs}`),this.progressBar("bg-green-500",100)):t()}).catch(()=>{s()}).finally(()=>{setTimeout(()=>{this.reset()},2e3)})}else this.msg("Enter code","text-red-500"),this.progressBar("bg-red-500",100),setTimeout(()=>{this.reset()},2e3)}})rawliteral";

