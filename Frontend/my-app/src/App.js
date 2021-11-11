import React from 'react';
import './App.css';
class App extends React.Component {
  constructor(){
    super();
    this.state={
      waitingForResponse: false,
      response: 'default response'
    }
  }
  sendData(){
    console.log('trying to send data')
    this.setState({
      waitingForResponse: true,
      response: 'waiting for response'
    });
    fetch('http://localhost:5000/send_data/',{
      method: 'POST',  
      credentials: 'include',
      body: JSON.stringify({
        hash: document.querySelector('.hash_input').value,
        hash_type: document.querySelector('.hash_type_input').value
      })
    })
    .then(function(resp){
      console.log('converting response to json');
      return resp.json();
    })
    .then(function(resp){
      this.setState({
        waitingForResponse: false,
        response: resp['plaintext']
      });
      console.log(resp['plaintext']);
    }.bind(this))
    .catch(function(e){
      console.log('error encountered');
    })
  }
  getOutput(){
    if(this.state.waitingForResponse){
      return 'waiting for response';
    }
    else{
      return this.state.response;
    }
  }
  render(){
    return (
    <div className="App">
      	<h1>Hash Cracking Utility</h1>
      	<hr></hr>
      	<br></br>
      	
      	
      	<form className="Hash">
      		<h2>Please provide the HashValue</h2> 
      		<input type="text" className="hash_input"></input>  <br/>
      		<h2>Choose The Hash Type</h2>          
      		<select name="type" id="type" className="hash_type_input">
				  <option value="sha256">SHA256</option>
				  <option value="md5">MD5</option>
				  <option value="sha512">SHA512</option>
				  <option value="ntlm">NTLM</option>
				</select>
      		<input type="button" value="Submit" onClick={this.sendData.bind(this)}>
      		</input>
      	</form> 	
        <div> {this.state.response} </div>
    </div>
  );
  }
}

export default App;