// Code generated - DO NOT EDIT.
// This file is a generated binding and any manual changes will be lost.

package u2sso

import (
	"errors"
	"math/big"
	"strings"

	ethereum "github.com/ethereum/go-ethereum"
	"github.com/ethereum/go-ethereum/accounts/abi"
	"github.com/ethereum/go-ethereum/accounts/abi/bind"
	"github.com/ethereum/go-ethereum/common"
	"github.com/ethereum/go-ethereum/core/types"
	"github.com/ethereum/go-ethereum/event"
)

// Reference imports to suppress errors if they are not otherwise used.
var (
	_ = errors.New
	_ = big.NewInt
	_ = strings.NewReader
	_ = ethereum.NotFound
	_ = bind.Bind
	_ = common.Big1
	_ = types.BloomLookup
	_ = event.NewSubscription
	_ = abi.ConvertType
)

// U2ssoMetaData contains all meta data concerning the U2sso contract.
var U2ssoMetaData = &bind.MetaData{
	ABI: "[{\"inputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"constructor\"},{\"inputs\":[{\"internalType\":\"uint256\",\"name\":\"_id\",\"type\":\"uint256\"},{\"internalType\":\"uint256\",\"name\":\"_id33\",\"type\":\"uint256\"}],\"name\":\"addID\",\"outputs\":[{\"internalType\":\"uint256\",\"name\":\"\",\"type\":\"uint256\"}],\"stateMutability\":\"nonpayable\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"uint256\",\"name\":\"_id\",\"type\":\"uint256\"},{\"internalType\":\"uint256\",\"name\":\"_id33\",\"type\":\"uint256\"}],\"name\":\"getIDIndex\",\"outputs\":[{\"internalType\":\"int256\",\"name\":\"\",\"type\":\"int256\"}],\"stateMutability\":\"view\",\"type\":\"function\"},{\"inputs\":[],\"name\":\"getIDSize\",\"outputs\":[{\"internalType\":\"uint256\",\"name\":\"\",\"type\":\"uint256\"}],\"stateMutability\":\"view\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"uint256\",\"name\":\"_index\",\"type\":\"uint256\"}],\"name\":\"getIDs\",\"outputs\":[{\"internalType\":\"uint256\",\"name\":\"\",\"type\":\"uint256\"},{\"internalType\":\"uint256\",\"name\":\"\",\"type\":\"uint256\"}],\"stateMutability\":\"view\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"uint256\",\"name\":\"_index\",\"type\":\"uint256\"}],\"name\":\"getState\",\"outputs\":[{\"internalType\":\"bool\",\"name\":\"\",\"type\":\"bool\"}],\"stateMutability\":\"view\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"uint256\",\"name\":\"\",\"type\":\"uint256\"}],\"name\":\"idList\",\"outputs\":[{\"internalType\":\"uint256\",\"name\":\"id\",\"type\":\"uint256\"},{\"internalType\":\"uint256\",\"name\":\"id33\",\"type\":\"uint256\"},{\"internalType\":\"bool\",\"name\":\"active\",\"type\":\"bool\"},{\"internalType\":\"address\",\"name\":\"owner\",\"type\":\"address\"}],\"stateMutability\":\"view\",\"type\":\"function\"},{\"inputs\":[{\"internalType\":\"uint256\",\"name\":\"_index\",\"type\":\"uint256\"}],\"name\":\"revokeID\",\"outputs\":[],\"stateMutability\":\"nonpayable\",\"type\":\"function\"}]",
}

// U2ssoABI is the input ABI used to generate the binding from.
// Deprecated: Use U2ssoMetaData.ABI instead.
var U2ssoABI = U2ssoMetaData.ABI

// U2sso is an auto generated Go binding around an Ethereum contract.
type U2sso struct {
	U2ssoCaller     // Read-only binding to the contract
	U2ssoTransactor // Write-only binding to the contract
	U2ssoFilterer   // Log filterer for contract events
}

// U2ssoCaller is an auto generated read-only Go binding around an Ethereum contract.
type U2ssoCaller struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// U2ssoTransactor is an auto generated write-only Go binding around an Ethereum contract.
type U2ssoTransactor struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// U2ssoFilterer is an auto generated log filtering Go binding around an Ethereum contract events.
type U2ssoFilterer struct {
	contract *bind.BoundContract // Generic contract wrapper for the low level calls
}

// U2ssoSession is an auto generated Go binding around an Ethereum contract,
// with pre-set call and transact options.
type U2ssoSession struct {
	Contract     *U2sso            // Generic contract binding to set the session for
	CallOpts     bind.CallOpts     // Call options to use throughout this session
	TransactOpts bind.TransactOpts // Transaction auth options to use throughout this session
}

// U2ssoCallerSession is an auto generated read-only Go binding around an Ethereum contract,
// with pre-set call options.
type U2ssoCallerSession struct {
	Contract *U2ssoCaller  // Generic contract caller binding to set the session for
	CallOpts bind.CallOpts // Call options to use throughout this session
}

// U2ssoTransactorSession is an auto generated write-only Go binding around an Ethereum contract,
// with pre-set transact options.
type U2ssoTransactorSession struct {
	Contract     *U2ssoTransactor  // Generic contract transactor binding to set the session for
	TransactOpts bind.TransactOpts // Transaction auth options to use throughout this session
}

// U2ssoRaw is an auto generated low-level Go binding around an Ethereum contract.
type U2ssoRaw struct {
	Contract *U2sso // Generic contract binding to access the raw methods on
}

// U2ssoCallerRaw is an auto generated low-level read-only Go binding around an Ethereum contract.
type U2ssoCallerRaw struct {
	Contract *U2ssoCaller // Generic read-only contract binding to access the raw methods on
}

// U2ssoTransactorRaw is an auto generated low-level write-only Go binding around an Ethereum contract.
type U2ssoTransactorRaw struct {
	Contract *U2ssoTransactor // Generic write-only contract binding to access the raw methods on
}

// NewU2sso creates a new instance of U2sso, bound to a specific deployed contract.
func NewU2sso(address common.Address, backend bind.ContractBackend) (*U2sso, error) {
	contract, err := bindU2sso(address, backend, backend, backend)
	if err != nil {
		return nil, err
	}
	return &U2sso{U2ssoCaller: U2ssoCaller{contract: contract}, U2ssoTransactor: U2ssoTransactor{contract: contract}, U2ssoFilterer: U2ssoFilterer{contract: contract}}, nil
}

// NewU2ssoCaller creates a new read-only instance of U2sso, bound to a specific deployed contract.
func NewU2ssoCaller(address common.Address, caller bind.ContractCaller) (*U2ssoCaller, error) {
	contract, err := bindU2sso(address, caller, nil, nil)
	if err != nil {
		return nil, err
	}
	return &U2ssoCaller{contract: contract}, nil
}

// NewU2ssoTransactor creates a new write-only instance of U2sso, bound to a specific deployed contract.
func NewU2ssoTransactor(address common.Address, transactor bind.ContractTransactor) (*U2ssoTransactor, error) {
	contract, err := bindU2sso(address, nil, transactor, nil)
	if err != nil {
		return nil, err
	}
	return &U2ssoTransactor{contract: contract}, nil
}

// NewU2ssoFilterer creates a new log filterer instance of U2sso, bound to a specific deployed contract.
func NewU2ssoFilterer(address common.Address, filterer bind.ContractFilterer) (*U2ssoFilterer, error) {
	contract, err := bindU2sso(address, nil, nil, filterer)
	if err != nil {
		return nil, err
	}
	return &U2ssoFilterer{contract: contract}, nil
}

// bindU2sso binds a generic wrapper to an already deployed contract.
func bindU2sso(address common.Address, caller bind.ContractCaller, transactor bind.ContractTransactor, filterer bind.ContractFilterer) (*bind.BoundContract, error) {
	parsed, err := U2ssoMetaData.GetAbi()
	if err != nil {
		return nil, err
	}
	return bind.NewBoundContract(address, *parsed, caller, transactor, filterer), nil
}

// Call invokes the (constant) contract method with params as input values and
// sets the output to result. The result type might be a single field for simple
// returns, a slice of interfaces for anonymous returns and a struct for named
// returns.
func (_U2sso *U2ssoRaw) Call(opts *bind.CallOpts, result *[]interface{}, method string, params ...interface{}) error {
	return _U2sso.Contract.U2ssoCaller.contract.Call(opts, result, method, params...)
}

// Transfer initiates a plain transaction to move funds to the contract, calling
// its default method if one is available.
func (_U2sso *U2ssoRaw) Transfer(opts *bind.TransactOpts) (*types.Transaction, error) {
	return _U2sso.Contract.U2ssoTransactor.contract.Transfer(opts)
}

// Transact invokes the (paid) contract method with params as input values.
func (_U2sso *U2ssoRaw) Transact(opts *bind.TransactOpts, method string, params ...interface{}) (*types.Transaction, error) {
	return _U2sso.Contract.U2ssoTransactor.contract.Transact(opts, method, params...)
}

// Call invokes the (constant) contract method with params as input values and
// sets the output to result. The result type might be a single field for simple
// returns, a slice of interfaces for anonymous returns and a struct for named
// returns.
func (_U2sso *U2ssoCallerRaw) Call(opts *bind.CallOpts, result *[]interface{}, method string, params ...interface{}) error {
	return _U2sso.Contract.contract.Call(opts, result, method, params...)
}

// Transfer initiates a plain transaction to move funds to the contract, calling
// its default method if one is available.
func (_U2sso *U2ssoTransactorRaw) Transfer(opts *bind.TransactOpts) (*types.Transaction, error) {
	return _U2sso.Contract.contract.Transfer(opts)
}

// Transact invokes the (paid) contract method with params as input values.
func (_U2sso *U2ssoTransactorRaw) Transact(opts *bind.TransactOpts, method string, params ...interface{}) (*types.Transaction, error) {
	return _U2sso.Contract.contract.Transact(opts, method, params...)
}

// GetIDIndex is a free data retrieval call binding the contract method 0x9a80d1dd.
//
// Solidity: function getIDIndex(uint256 _id, uint256 _id33) view returns(int256)
func (_U2sso *U2ssoCaller) GetIDIndex(opts *bind.CallOpts, _id *big.Int, _id33 *big.Int) (*big.Int, error) {
	var out []interface{}
	err := _U2sso.contract.Call(opts, &out, "getIDIndex", _id, _id33)

	if err != nil {
		return *new(*big.Int), err
	}

	out0 := *abi.ConvertType(out[0], new(*big.Int)).(**big.Int)

	return out0, err

}

// GetIDIndex is a free data retrieval call binding the contract method 0x9a80d1dd.
//
// Solidity: function getIDIndex(uint256 _id, uint256 _id33) view returns(int256)
func (_U2sso *U2ssoSession) GetIDIndex(_id *big.Int, _id33 *big.Int) (*big.Int, error) {
	return _U2sso.Contract.GetIDIndex(&_U2sso.CallOpts, _id, _id33)
}

// GetIDIndex is a free data retrieval call binding the contract method 0x9a80d1dd.
//
// Solidity: function getIDIndex(uint256 _id, uint256 _id33) view returns(int256)
func (_U2sso *U2ssoCallerSession) GetIDIndex(_id *big.Int, _id33 *big.Int) (*big.Int, error) {
	return _U2sso.Contract.GetIDIndex(&_U2sso.CallOpts, _id, _id33)
}

// GetIDSize is a free data retrieval call binding the contract method 0x2d3d1104.
//
// Solidity: function getIDSize() view returns(uint256)
func (_U2sso *U2ssoCaller) GetIDSize(opts *bind.CallOpts) (*big.Int, error) {
	var out []interface{}
	err := _U2sso.contract.Call(opts, &out, "getIDSize")

	if err != nil {
		return *new(*big.Int), err
	}

	out0 := *abi.ConvertType(out[0], new(*big.Int)).(**big.Int)

	return out0, err

}

// GetIDSize is a free data retrieval call binding the contract method 0x2d3d1104.
//
// Solidity: function getIDSize() view returns(uint256)
func (_U2sso *U2ssoSession) GetIDSize() (*big.Int, error) {
	return _U2sso.Contract.GetIDSize(&_U2sso.CallOpts)
}

// GetIDSize is a free data retrieval call binding the contract method 0x2d3d1104.
//
// Solidity: function getIDSize() view returns(uint256)
func (_U2sso *U2ssoCallerSession) GetIDSize() (*big.Int, error) {
	return _U2sso.Contract.GetIDSize(&_U2sso.CallOpts)
}

// GetIDs is a free data retrieval call binding the contract method 0x6f1acd98.
//
// Solidity: function getIDs(uint256 _index) view returns(uint256, uint256)
func (_U2sso *U2ssoCaller) GetIDs(opts *bind.CallOpts, _index *big.Int) (*big.Int, *big.Int, error) {
	var out []interface{}
	err := _U2sso.contract.Call(opts, &out, "getIDs", _index)

	if err != nil {
		return *new(*big.Int), *new(*big.Int), err
	}

	out0 := *abi.ConvertType(out[0], new(*big.Int)).(**big.Int)
	out1 := *abi.ConvertType(out[1], new(*big.Int)).(**big.Int)

	return out0, out1, err

}

// GetIDs is a free data retrieval call binding the contract method 0x6f1acd98.
//
// Solidity: function getIDs(uint256 _index) view returns(uint256, uint256)
func (_U2sso *U2ssoSession) GetIDs(_index *big.Int) (*big.Int, *big.Int, error) {
	return _U2sso.Contract.GetIDs(&_U2sso.CallOpts, _index)
}

// GetIDs is a free data retrieval call binding the contract method 0x6f1acd98.
//
// Solidity: function getIDs(uint256 _index) view returns(uint256, uint256)
func (_U2sso *U2ssoCallerSession) GetIDs(_index *big.Int) (*big.Int, *big.Int, error) {
	return _U2sso.Contract.GetIDs(&_U2sso.CallOpts, _index)
}

// GetState is a free data retrieval call binding the contract method 0x44c9af28.
//
// Solidity: function getState(uint256 _index) view returns(bool)
func (_U2sso *U2ssoCaller) GetState(opts *bind.CallOpts, _index *big.Int) (bool, error) {
	var out []interface{}
	err := _U2sso.contract.Call(opts, &out, "getState", _index)

	if err != nil {
		return *new(bool), err
	}

	out0 := *abi.ConvertType(out[0], new(bool)).(*bool)

	return out0, err

}

// GetState is a free data retrieval call binding the contract method 0x44c9af28.
//
// Solidity: function getState(uint256 _index) view returns(bool)
func (_U2sso *U2ssoSession) GetState(_index *big.Int) (bool, error) {
	return _U2sso.Contract.GetState(&_U2sso.CallOpts, _index)
}

// GetState is a free data retrieval call binding the contract method 0x44c9af28.
//
// Solidity: function getState(uint256 _index) view returns(bool)
func (_U2sso *U2ssoCallerSession) GetState(_index *big.Int) (bool, error) {
	return _U2sso.Contract.GetState(&_U2sso.CallOpts, _index)
}

// IdList is a free data retrieval call binding the contract method 0x6313531f.
//
// Solidity: function idList(uint256 ) view returns(uint256 id, uint256 id33, bool active, address owner)
func (_U2sso *U2ssoCaller) IdList(opts *bind.CallOpts, arg0 *big.Int) (struct {
	Id     *big.Int
	Id33   *big.Int
	Active bool
	Owner  common.Address
}, error) {
	var out []interface{}
	err := _U2sso.contract.Call(opts, &out, "idList", arg0)

	outstruct := new(struct {
		Id     *big.Int
		Id33   *big.Int
		Active bool
		Owner  common.Address
	})
	if err != nil {
		return *outstruct, err
	}

	outstruct.Id = *abi.ConvertType(out[0], new(*big.Int)).(**big.Int)
	outstruct.Id33 = *abi.ConvertType(out[1], new(*big.Int)).(**big.Int)
	outstruct.Active = *abi.ConvertType(out[2], new(bool)).(*bool)
	outstruct.Owner = *abi.ConvertType(out[3], new(common.Address)).(*common.Address)

	return *outstruct, err

}

// IdList is a free data retrieval call binding the contract method 0x6313531f.
//
// Solidity: function idList(uint256 ) view returns(uint256 id, uint256 id33, bool active, address owner)
func (_U2sso *U2ssoSession) IdList(arg0 *big.Int) (struct {
	Id     *big.Int
	Id33   *big.Int
	Active bool
	Owner  common.Address
}, error) {
	return _U2sso.Contract.IdList(&_U2sso.CallOpts, arg0)
}

// IdList is a free data retrieval call binding the contract method 0x6313531f.
//
// Solidity: function idList(uint256 ) view returns(uint256 id, uint256 id33, bool active, address owner)
func (_U2sso *U2ssoCallerSession) IdList(arg0 *big.Int) (struct {
	Id     *big.Int
	Id33   *big.Int
	Active bool
	Owner  common.Address
}, error) {
	return _U2sso.Contract.IdList(&_U2sso.CallOpts, arg0)
}

// AddID is a paid mutator transaction binding the contract method 0x72b8dee1.
//
// Solidity: function addID(uint256 _id, uint256 _id33) returns(uint256)
func (_U2sso *U2ssoTransactor) AddID(opts *bind.TransactOpts, _id *big.Int, _id33 *big.Int) (*types.Transaction, error) {
	return _U2sso.contract.Transact(opts, "addID", _id, _id33)
}

// AddID is a paid mutator transaction binding the contract method 0x72b8dee1.
//
// Solidity: function addID(uint256 _id, uint256 _id33) returns(uint256)
func (_U2sso *U2ssoSession) AddID(_id *big.Int, _id33 *big.Int) (*types.Transaction, error) {
	return _U2sso.Contract.AddID(&_U2sso.TransactOpts, _id, _id33)
}

// AddID is a paid mutator transaction binding the contract method 0x72b8dee1.
//
// Solidity: function addID(uint256 _id, uint256 _id33) returns(uint256)
func (_U2sso *U2ssoTransactorSession) AddID(_id *big.Int, _id33 *big.Int) (*types.Transaction, error) {
	return _U2sso.Contract.AddID(&_U2sso.TransactOpts, _id, _id33)
}

// RevokeID is a paid mutator transaction binding the contract method 0xce3375d5.
//
// Solidity: function revokeID(uint256 _index) returns()
func (_U2sso *U2ssoTransactor) RevokeID(opts *bind.TransactOpts, _index *big.Int) (*types.Transaction, error) {
	return _U2sso.contract.Transact(opts, "revokeID", _index)
}

// RevokeID is a paid mutator transaction binding the contract method 0xce3375d5.
//
// Solidity: function revokeID(uint256 _index) returns()
func (_U2sso *U2ssoSession) RevokeID(_index *big.Int) (*types.Transaction, error) {
	return _U2sso.Contract.RevokeID(&_U2sso.TransactOpts, _index)
}

// RevokeID is a paid mutator transaction binding the contract method 0xce3375d5.
//
// Solidity: function revokeID(uint256 _index) returns()
func (_U2sso *U2ssoTransactorSession) RevokeID(_index *big.Int) (*types.Transaction, error) {
	return _U2sso.Contract.RevokeID(&_U2sso.TransactOpts, _index)
}
